#include "circuitgate.h"
#include "circuitwire.h"

#include <cassert>

namespace quartz {
int CircuitGate::get_min_qubit_index() const {
  int result = -1;
  for (auto &input_node : input_wires) {
    if (input_node->is_qubit() &&
        (result == -1 || input_node->index < result)) {
      result = input_node->index;
    }
  }
  return result;
}

std::vector<int> CircuitGate::get_qubit_indices() const {
  std::vector<int> result;
  result.reserve(gate->get_num_qubits());
  for (auto &input_node : input_wires) {
    if (input_node->is_qubit()) {
      result.push_back(input_node->index);
    }
  }
  return result;
}

std::vector<int> CircuitGate::get_control_qubit_indices() const {
  const int num_control_qubits = gate->get_num_control_qubits();
  if (num_control_qubits == 0) {
    return std::vector<int>();
  }
  std::vector<int> result;
  result.reserve(num_control_qubits);
  for (auto &input_node : input_wires) {
    // Control qubits always appear first.
    if (input_node->is_qubit()) {
      result.push_back(input_node->index);
      if (result.size() == num_control_qubits) {
        break;
      }
    }
  }
  return result;
}

std::string CircuitGate::to_string() const {
  std::string result;
  if (output_wires.size() == 1) {
    result += output_wires[0]->to_string();
  } else if (output_wires.size() == 2) {
    result += "[" + output_wires[0]->to_string();
    result += ", " + output_wires[1]->to_string();
    result += "]";
  } else {
    assert(false && "A circuit gate should have 1 or 2 outputs.");
  }
  result += " = ";
  result += gate_type_name(gate->tp);
  result += "(";
  for (int j = 0; j < (int)input_wires.size(); j++) {
    result += input_wires[j]->to_string();
    if (j != (int)input_wires.size() - 1) {
      result += ", ";
    }
  }
  result += ")";
  return result;
}

} // namespace quartz