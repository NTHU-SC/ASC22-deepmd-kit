#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "AtomMap.h"
#include "errors.h"
#include "neighbor_list.h"
#include "version.h"

#ifdef TF_PRIVATE
#include "tf_private.h"
#else
#include "tf_public.h"
#endif

namespace deepmd {

typedef double ENERGYTYPE;

struct NeighborListData {
  /// Array stores the core region atom's index
  std::vector<int> ilist;
  /// Array stores the core region atom's neighbor index
  std::vector<std::vector<int>> jlist;
  /// Array stores the number of neighbors of core region atoms
  std::vector<int> numneigh;
  /// Array stores the the location of the first neighbor of core region atoms
  std::vector<int*> firstneigh;

 public:
  void copy_from_nlist(const InputNlist& inlist);
  void shuffle(const std::vector<int>& fwd_map);
  void shuffle(const deepmd::AtomMap& map);
  void shuffle_exclude_empty(const std::vector<int>& fwd_map);
  void make_inlist(InputNlist& inlist);
};

/**
 * @brief Check if the model version is supported.
 * @param[in] model_version The model version.
 * @return Whether the model is supported (true or false).
 **/
bool model_compatable(std::string& model_version);

template <typename VALUETYPE>
void select_by_type(std::vector<int>& fwd_map,
                    std::vector<int>& bkw_map,
                    int& nghost_real,
                    const std::vector<VALUETYPE>& dcoord_,
                    const std::vector<int>& datype_,
                    const int& nghost,
                    const std::vector<int>& sel_type_);

template <typename VALUETYPE>
void select_real_atoms(std::vector<int>& fwd_map,
                       std::vector<int>& bkw_map,
                       int& nghost_real,
                       const std::vector<VALUETYPE>& dcoord_,
                       const std::vector<int>& datype_,
                       const int& nghost,
                       const int& ntypes);

template <typename VT>
void select_map(std::vector<VT>& out,
                const std::vector<VT>& in,
                const std::vector<int>& fwd_map,
                const int& stride,
                const int& nframes = 1,
                // nall will not take effect if nframes is 1
                const int& nall1 = 0,
                const int& nall2 = 0);

template <typename VT>
void select_map(typename std::vector<VT>::iterator out,
                const typename std::vector<VT>::const_iterator in,
                const std::vector<int>& fwd_map,
                const int& stride,
                const int& nframes = 1,
                const int& nall1 = 0,
                const int& nall2 = 0);

template <typename VT>
void select_map_inv(std::vector<VT>& out,
                    const std::vector<VT>& in,
                    const std::vector<int>& fwd_map,
                    const int& stride);

template <typename VT>
void select_map_inv(typename std::vector<VT>::iterator out,
                    const typename std::vector<VT>::const_iterator in,
                    const std::vector<int>& fwd_map,
                    const int& stride);

/**
 * @brief Get the number of threads from the environment variable.
 * @details A warning will be thrown if environmental variables are not set.
 * @param[out] num_intra_nthreads The number of intra threads. Read from
 *TF_INTRA_OP_PARALLELISM_THREADS.
 * @param[out] num_inter_nthreads The number of inter threads. Read from
 *TF_INTER_OP_PARALLELISM_THREADS.
 **/
void get_env_nthreads(int& num_intra_nthreads, int& num_inter_nthreads);

/**
 * @brief Dynamically load OP library. This should be called before loading
 * graphs.
 */
void load_op_library();

/** @struct deepmd::deepmd_exception
 **/

/**
 * @brief Throw exception if TensorFlow doesn't work.
 **/
struct tf_exception : public deepmd::deepmd_exception {
 public:
  tf_exception() : deepmd::deepmd_exception("TensorFlow Error!"){};
  tf_exception(const std::string& msg)
      : deepmd::deepmd_exception(std::string("TensorFlow Error: ") + msg){};
};

/**
 * @brief Check TensorFlow status. Exit if not OK.
 * @param[in] status TensorFlow status.
 **/
void check_status(const tensorflow::Status& status);

std::string name_prefix(const std::string& name_scope);

/**
 * @brief Get the value of a tensor.
 * @param[in] session TensorFlow session.
 * @param[in] name The name of the tensor.
 * @param[in] scope The scope of the tensor.
 * @return The value of the tensor.
 **/
template <typename VT>
VT session_get_scalar(tensorflow::Session* session,
                      const std::string name,
                      const std::string scope = "");

/**
 * @brief Get the vector of a tensor.
 * @param[out] o_vec The output vector.
 * @param[in] session TensorFlow session.
 * @param[in] name The name of the tensor.
 * @param[in] scope The scope of the tensor.
 **/
template <typename VT>
void session_get_vector(std::vector<VT>& o_vec,
                        tensorflow::Session* session,
                        const std::string name_,
                        const std::string scope = "");

/**
 * @brief Get the type of a tensor.
 * @param[in] session TensorFlow session.
 * @param[in] name The name of the tensor.
 * @param[in] scope The scope of the tensor.
 * @return The type of the tensor as int.
 **/
int session_get_dtype(tensorflow::Session* session,
                      const std::string name,
                      const std::string scope = "");

/**
 * @brief Get input tensors.
 * @param[out] input_tensors Input tensors.
 * @param[in] dcoord_ Coordinates of atoms.
 * @param[in] ntypes Number of atom types.
 * @param[in] datype_ Atom types.
 * @param[in] dbox Box matrix.
 * @param[in] cell_size Cell size.
 * @param[in] fparam_ Frame parameters.
 * @param[in] aparam_ Atom parameters.
 * @param[in] atommap Atom map.
 * @param[in] scope The scope of the tensors.
 */
template <typename MODELTYPE, typename VALUETYPE>
int session_input_tensors(
    std::vector<std::pair<std::string, tensorflow::Tensor>>& input_tensors,
    const std::vector<VALUETYPE>& dcoord_,
    const int& ntypes,
    const std::vector<int>& datype_,
    const std::vector<VALUETYPE>& dbox,
    const double& cell_size,
    const std::vector<VALUETYPE>& fparam_,
    const std::vector<VALUETYPE>& aparam_,
    const deepmd::AtomMap& atommap,
    const std::string scope = "");

/**
 * @brief Get input tensors.
 * @param[out] input_tensors Input tensors.
 * @param[in] dcoord_ Coordinates of atoms.
 * @param[in] ntypes Number of atom types.
 * @param[in] datype_ Atom types.
 * @param[in] dlist Neighbor list.
 * @param[in] fparam_ Frame parameters.
 * @param[in] aparam_ Atom parameters.
 * @param[in] atommap Atom map.
 * @param[in] nghost Number of ghost atoms.
 * @param[in] ago Update the internal neighbour list if ago is 0.
 * @param[in] scope The scope of the tensors.
 */
template <typename MODELTYPE, typename VALUETYPE>
int session_input_tensors(
    std::vector<std::pair<std::string, tensorflow::Tensor>>& input_tensors,
    const std::vector<VALUETYPE>& dcoord_,
    const int& ntypes,
    const std::vector<int>& datype_,
    const std::vector<VALUETYPE>& dbox,
    InputNlist& dlist,
    const std::vector<VALUETYPE>& fparam_,
    const std::vector<VALUETYPE>& aparam_,
    const deepmd::AtomMap& atommap,
    const int nghost,
    const int ago,
    const std::string scope = "");

/**
 * @brief Get input tensors for mixed type.
 * @param[out] input_tensors Input tensors.
 * @param[in] nframes Number of frames.
 * @param[in] dcoord_ Coordinates of atoms.
 * @param[in] ntypes Number of atom types.
 * @param[in] datype_ Atom types.
 * @param[in] dlist Neighbor list.
 * @param[in] fparam_ Frame parameters.
 * @param[in] aparam_ Atom parameters.
 * @param[in] atommap Atom map.
 * @param[in] nghost Number of ghost atoms.
 * @param[in] ago Update the internal neighbour list if ago is 0.
 * @param[in] scope The scope of the tensors.
 */
template <typename MODELTYPE, typename VALUETYPE>
int session_input_tensors_mixed_type(
    std::vector<std::pair<std::string, tensorflow::Tensor>>& input_tensors,
    const int& nframes,
    const std::vector<VALUETYPE>& dcoord_,
    const int& ntypes,
    const std::vector<int>& datype_,
    const std::vector<VALUETYPE>& dbox,
    const double& cell_size,
    const std::vector<VALUETYPE>& fparam_,
    const std::vector<VALUETYPE>& aparam_,
    const deepmd::AtomMap& atommap,
    const std::string scope = "");

/**
 * @brief Read model file to a string.
 * @param[in] model Path to the model.
 * @param[out] file_content Content of the model file.
 **/
void read_file_to_string(std::string model, std::string& file_content);

/**
 * @brief Convert pbtxt to pb.
 * @param[in] fn_pb_txt Filename of the pb txt file.
 * @param[in] fn_pb Filename of the pb file.
 **/
void convert_pbtxt_to_pb(std::string fn_pb_txt, std::string fn_pb);

/**
 * @brief Print the summary of DeePMD-kit, including the version and the build
 * information.
 * @param[in] pre The prefix to each line.
 */
void print_summary(const std::string& pre);
}  // namespace deepmd
