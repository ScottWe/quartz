#include "gate.h"

Gate::Gate(GateType tp, int num_qubits, int num_parameters)
    : tp(tp), num_qubits(num_qubits), num_parameters(num_parameters) {}

MatrixBase *Gate::get_matrix() {
  // Default: no matrix (for parameter gates).
  return nullptr;
}

MatrixBase *Gate::get_matrix(const std::vector<ParamType> &params) {
  // Default: no parameters.
  return get_matrix();
}

ParamType Gate::compute(const std::vector<ParamType> &input_params) {
  // Default: do no computation (for quantum gates).
  return 0;
}

int Gate::get_num_qubits() const {
  return num_qubits;
}

int Gate::get_num_parameters() const {
  return num_parameters;
}

bool Gate::is_parameter_gate() const {
  return num_qubits == 0;
}

bool Gate::is_quantum_gate() const {
  return num_qubits > 0;
}

bool Gate::is_parametrized_gate() const {
  return num_qubits > 0 && num_parameters > 0;
}