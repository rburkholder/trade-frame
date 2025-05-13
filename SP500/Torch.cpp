#include <iostream>

#include <torch/torch.h>

#include "Torch.hpp"

void TorchTest() {

  // Define the LSTM network
  struct LSTM : torch::nn::Module {
    torch::nn::LSTM lstm;
    torch::nn::Linear linear;

    LSTM(int input_size, int hidden_size, int num_layers, int num_classes) :
      lstm(torch::nn::LSTMOptions(input_size, hidden_size).num_layers(num_layers)),
      linear(hidden_size, num_classes) {
      register_module("lstm", lstm);
      register_module("linear", linear);
    }

    torch::Tensor forward(torch::Tensor x) {
      // Initialize hidden and cell states
      auto h0 = torch::zeros({lstm->options.num_layers(), x.size(1), lstm->options.hidden_size()});
      auto c0 = torch::zeros({lstm->options.num_layers(), x.size(1), lstm->options.hidden_size()});

      // Pass the input through the LSTM layer
      torch::Tensor out;
      std::tie(out, std::ignore) = lstm->forward(x, std::tuple( h0, c0 ) );

      // Pass the output of the LSTM layer through the linear layer
      out = linear->forward(out.reshape({-1, lstm->options.hidden_size()}));
      return out;
    }
  };

  // Define the hyperparameters
  int input_size = 28;
  int hidden_size = 128;
  int num_layers = 2;
  int num_classes = 10;
  int seq_length = 28;
  int batch_size = 64;

  // Create the LSTM network
  LSTM lstm(input_size, hidden_size, num_layers, num_classes);

  // Create a random input tensor
  auto input = torch::randn({seq_length, batch_size, input_size});

  // Pass the input tensor through the LSTM network
  auto output = lstm.forward(input);

  // Print the output tensor
  std::cout << output.sizes() << std::endl;
}