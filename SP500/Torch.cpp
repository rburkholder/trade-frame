#include <cmath>
#include <vector>
#include <iostream>

#include <torch/torch.h>

#include "Torch.hpp"

// extracted from google search ai.
// one compile time error with scoped variable
// one run time error (so far) with tensor sizing - to be fixed
//    Unhandled standard exception of type "N3c105ErrorE" with message "Expected hidden[0] size 2 20 32, got [2, 1, 32]

void TorchTest_v2() {

  // Define the LSTM model
  class LSTM : public torch::nn::Module {
  public:

    LSTM( int input_size, int hidden_size, int num_layers, int output_size )
    : lstm( torch::nn::LSTMOptions( input_size, hidden_size ).num_layers( num_layers ) )
    , linear( hidden_size, output_size )
    {
      register_module( "lstm", lstm );
      register_module( "linear", linear );
    }

    torch::Tensor forward( torch::Tensor x, std::tuple<torch::Tensor, torch::Tensor>& hidden_state ) {

      torch::Tensor out;

      // Pass the input through the LSTM layer
      std::tie( out, hidden_state ) = lstm->forward( x, hidden_state );

      // Pass the output of the LSTM layer through the linear layer
      // from v1:
      //torch::Tensor prediction = linear->forward( out.reshape( { -1, lstm->options.hidden_size() } ) );
      torch::Tensor prediction = linear->forward( out );
      return prediction;
    }

    std::tuple<torch::Tensor, torch::Tensor> init_hidden( int batch_size ) {
      torch::Tensor hidden_state = torch::zeros( { lstm->options.num_layers(), batch_size, lstm->options.hidden_size() } );
      torch::Tensor   cell_state = torch::zeros( { lstm->options.num_layers(), batch_size, lstm->options.hidden_size() } );
      return std::make_tuple( hidden_state, cell_state );
    }

  private:
    torch::nn::LSTM lstm;
    torch::nn::Linear linear;
  };

  // Hyperparameters
  int input_size = 1;
  int hidden_size = 32;
  int num_layers = 2;
  int output_size = 1;
  double learning_rate = 0.01;
  int num_epochs = 100;
  int sequence_length = 20;

  // Generate synthetic data (sine wave)
  int num_time_steps = 200;
  std::vector<float> time_series_data;
  for (int i = 0; i < num_time_steps; ++i) {
      time_series_data.push_back(std::sin(0.1 * i));
  }

  // Normalize the data
  float max_val = *std::max_element(time_series_data.begin(), time_series_data.end());
  for (float &val : time_series_data) {
    val /= max_val;
  }

  // Prepare data for LSTM
  std::vector<torch::Tensor> x_batches;
  std::vector<torch::Tensor> y_batches;

  for (int i = 0; i < num_time_steps - sequence_length; ++i) {
    std::vector<float> x_seq(time_series_data.begin() + i, time_series_data.begin() + i + sequence_length);
    std::vector<float> y_seq(time_series_data.begin() + i + 1, time_series_data.begin() + i + sequence_length + 1);

    x_batches.push_back(torch::from_blob(x_seq.data(), {1, (long long)sequence_length, 1}));
    y_batches.push_back(torch::from_blob(y_seq.data(), {1, (long long)sequence_length, 1}));
  }

  // Instantiate the model
  LSTM model( input_size, hidden_size, num_layers, output_size );

  // Loss and optimizer
  torch::nn::MSELoss criterion;
  torch::optim::Adam optimizer( model.parameters(), learning_rate );

  // Training loop
  for ( int epoch = 0; epoch < num_epochs; ++epoch ) {
    torch::Tensor loss;
    for (size_t i = 0; i < x_batches.size(); ++i) {
      // Initialize hidden state & cell state
      std::tuple<torch::Tensor, torch::Tensor> hidden_state = model.init_hidden( 1 ); // 1 is batch size

      // Forward pass
      torch::Tensor predictions = model.forward( x_batches[i], hidden_state );  // run time error here, as mentioned above
      loss = criterion( predictions, y_batches[i] );

      // Backward and optimize
      optimizer.zero_grad();
      loss.backward();
      optimizer.step();
    }

    if ( (epoch + 1) % 10 == 0 ) {
        std::cout << "Epoch [" << (epoch + 1) << "/" << num_epochs << "], Loss: " << loss.item<float>() << std::endl;
    }
  }

  // Prediction
  std::vector<float> future_predictions;
  torch::Tensor input_seq = x_batches.back();
  std::tuple<torch::Tensor, torch::Tensor> hidden_state = model.init_hidden(1);

  for (int i = 0; i < 10; ++i) {
    torch::Tensor prediction = model.forward(input_seq, hidden_state);
    future_predictions.push_back(prediction[0][sequence_length - 1][0].item<float>());

      // Update input sequence for next prediction
    std::vector<float> updated_seq;
    for (int j = 1; j < sequence_length; ++j) {
        updated_seq.push_back(input_seq[0][j][0].item<float>());
    }
    updated_seq.push_back(prediction[0][sequence_length - 1][0].item<float>());
    input_seq = torch::from_blob(updated_seq.data(), {1, (long long)sequence_length, 1});
  }

  std::cout << "Future Predictions:" << std::endl;
  for (float val : future_predictions) {
      std::cout << val * max_val << " ";
  }
  std::cout << std::endl;

}

// =====================================

void TorchTest_v1() {

  // Define the LSTM network
  struct LSTM : torch::nn::Module {

    torch::nn::LSTM lstm;
    torch::nn::Linear linear;

    LSTM( int input_size, int hidden_size, int num_layers, int num_features )
    : lstm(torch::nn::LSTMOptions(input_size, hidden_size).num_layers( num_layers ))
    , linear( hidden_size, num_features )
    {
      register_module( "lstm", lstm );
      register_module( "linear", linear );
    }

    torch::Tensor forward( torch::Tensor x ) {
      // Initialize hidden and cell states
      auto h0 = torch::zeros({lstm->options.num_layers(), x.size(1), lstm->options.hidden_size() });
      auto c0 = torch::zeros({lstm->options.num_layers(), x.size(1), lstm->options.hidden_size() });

      // Pass the input through the LSTM layer
      torch::Tensor out;
      std::tie( out, std::ignore ) = lstm->forward(x, std::tuple( h0, c0 ) );

      // Pass the output of the LSTM layer through the linear layer
      out = linear->forward( out.reshape( { -1, lstm->options.hidden_size() } ) );
      return out;
    }
  };

  // Define the hyperparameters
  const int input_size = 28;
  const int hidden_size = 128;
  const int num_layers = 2;
  const int num_features = 10;
  const int seq_length = 28;
  const int batch_size = 64;

  // Create the LSTM network
  LSTM lstm( input_size, hidden_size, num_layers, num_features );

  // Create a random input tensor
  auto input = torch::randn( { seq_length, batch_size, input_size } );

  // Pass the input tensor through the LSTM network
  auto output = lstm.forward( input );

  // Print the output tensor
  std::cout << output.sizes() << std::endl;
}