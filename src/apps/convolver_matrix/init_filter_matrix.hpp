/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_APPS_CONVOLVER_MATRIX_INIT_FILTER_MATRIX_HPP_INCLUDED
#define VISR_APPS_CONVOLVER_MATRIX_INIT_FILTER_MATRIX_HPP_INCLUDED

#include <string>
#include <vector>

namespace visr
{
// Forward declaration
namespace efl
{
template< typename DataType >
class BasicMatrix;
}
namespace pml
{
class IndexSequence;
}

namespace apps
{
namespace convolver_matrix
{

/**
 * Initialise the matrix of impulse responses from a set of parameters.
 * @param filterList A comma-separated string containing the filter files(WAVs) 
 * to be read. Empty or blank if no files are to be loaded.
 * @param maxFilterLength The length of the filters. If provided, the WAV files are checked against this value, 
 * and padded with zeros. To ignore this value, use std::numeric_limits<std::size_t>::max(). In this case, the 
 * filter length is set to the length of the longest WAV file.
 * @param maxFilterEntries The number of filters in the returned matrix. An error is generated if the highest filter index exceeds this value.
 * Use std::numeric_limits<std::size_t>::max() to ignore this parameter. In this case, the number of filters 
 * is determined from the highest position index of the impulse responses.
 * @param indexOffsets Starting indices for the irs from the WAV files. If specified, it must has the same number of values 
 * as the number of files in \p  filterList. To ignore, pass an empty sequence. In this case, the IRs care stored consecutively in the output matrix.
 * @param [out] matrix The resized and filled impulse reesponse matrix.
 * @throw std::invalid_argument In case of inconsistent parameters.
 */
template< typename DataType >
void initFilterMatrix( std::string const & filterList,
                       std::size_t maxFilterLength,
                       std::size_t maxFilterEntries,
                       pml::IndexSequence const & indexOffsets,
                       efl::BasicMatrix<DataType> & matrix );

} // namespace convolver_matrix
} // namespace apps
} // namespace visr

#endif // #ifndef VISR_APPS_CONVOLVER_MATRIX_INIT_FILTER_MATRIX_HPP_INCLUDED
