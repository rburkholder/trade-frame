#include <iostream>

#include <torch/torch.h>

#include "Torch.hpp"

void TorchTest() {
  torch::Tensor tensor = torch::rand({2, 3});
  std::cout << tensor << std::endl;
}