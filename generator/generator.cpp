#include "generator.h"

#include <cassert>

void Generator::generate(int num_qubits,
                         int max_num_parameters,
                         int max_num_gates)
{
  DAG* dag = new DAG(num_qubits, max_num_parameters);
  std::unordered_map<size_t, std::unordered_set<DAG*> > dataset;
  std::vector<bool> used_parameters(max_num_parameters, false);
  dfs(0, max_num_gates, dag, used_parameters, dataset);
  for (const auto& it : dataset) {
    for (const auto& dag : it.second) {
      printf("key = %zu \n", dag->hash(context));
      dag->print(context);
    }
  }
}

void Generator::dfs(int gate_idx,
                    int max_num_gates,
                    DAG *dag,
                    std::vector<bool> &used_parameters,
                    std::unordered_map<size_t,
                                       std::unordered_set<DAG *> > &dataset)
{
  bool pass_checks = true;
  // check that qubits are used in an increasing order
  for (int i = 1; i < dag->get_num_qubits(); i++)
    if (dag->outputs[i] == dag->nodes[i].get()
    && dag->outputs[i-1] != dag->nodes[i-1].get())
      pass_checks = false;
  // check that parameters are used in an increasing order
  for (int i = 1; i < dag->get_num_input_parameters(); i++)
    if (used_parameters[i] && !used_parameters[i-1])
      pass_checks = false;
  // Return if we fail any checks
  if (!pass_checks)
    return;

  // save a clone of dag to dataset
  dataset[dag->hash(context)].insert(new DAG(*dag));

  if (gate_idx > max_num_gates)
    return;
  std::vector<int> qubit_indices;
  std::vector<int> parameter_indices;
  for (const auto& idx : context->get_supported_gates()) {
    Gate* gate = context->get_gate(idx);
    if (gate->get_num_qubits() == 1) {
      if (gate->get_num_parameters() == 0) {
        // Case: 1-qubit operators without parameters
        for (int i = 0; i < dag->get_num_qubits(); i++) {
          qubit_indices.push_back(i);
          bool ret = dag->add_gate(qubit_indices, parameter_indices, gate, NULL);
	  assert(ret);
          dfs(gate_idx+1, max_num_gates, dag, used_parameters, dataset);
          ret = dag->remove_last_gate();
	  assert(ret);
	  assert(ret);
          qubit_indices.pop_back();
        }
      } else if (gate->get_num_parameters() == 1) {
        // Case: 1-qubit operators with 1 parameter
        for (int q1 = 0; q1 < dag->get_num_qubits(); q1++) {
          qubit_indices.push_back(q1);
          for (int p1 = 0; p1 < dag->get_num_total_parameters(); p1++) {
            parameter_indices.push_back(p1);
            bool ret = dag->add_gate(qubit_indices, parameter_indices, gate, NULL);
	    assert(ret);
            bool old_used_p1 = false;
            if (p1 < dag->get_num_input_parameters()) {
              old_used_p1 = used_parameters[p1];
              used_parameters[p1] = true;
            }
            dfs(gate_idx+1, max_num_gates, dag, used_parameters, dataset);
            if (p1 < dag->get_num_input_parameters()) {
              used_parameters[p1] = old_used_p1;
            }
            ret = dag->remove_last_gate();
	    assert(ret);
            parameter_indices.pop_back();
          }
          qubit_indices.pop_back();
        }
      } else {
        assert(false && "To be implemented...");
      }
    } else if (gate->get_num_qubits() == 2) {
      if (gate->get_num_parameters() == 0) {
        // Case: 2-qubit operators without parameters
        for (int q1 = 0; q1 < dag->get_num_qubits(); q1++) {
          qubit_indices.push_back(q1);
          for (int q2 = 0; q2 < dag->get_num_qubits(); q2++) {
            if (q1 == q2) continue;
            qubit_indices.push_back(q2);
            bool ret = dag->add_gate(qubit_indices, parameter_indices, gate, NULL);
	    assert(ret);
            dfs(gate_idx+1, max_num_gates, dag, used_parameters, dataset);
            ret = dag->remove_last_gate();
	    assert(ret);
            qubit_indices.pop_back();
          }
          qubit_indices.pop_back();
        }
      } else {
        assert(false && "To be implemented...");
      }
    } else {
      // Current only support 1- and 2-qubit gates
      assert(false && "Unsupported gate");
    }
  }
}
