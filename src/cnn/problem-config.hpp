/* Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <iomanip>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "util/dynamic-array.hpp"
#include "loop-analysis/point-set.hpp"

namespace problem
{

enum class DataType : unsigned int
{
  Weight,
  Input,
  Output,
  Num
};

extern std::map<std::string, DataType> DataTypeID;
extern std::map<DataType, std::string> DataTypeName;

std::ostream& operator<<(std::ostream& out, const DataType& d);
bool IsReadWriteDataType(const DataType d);


// Think of this as std::array<T, DataType::Num>, except that the goal is
// to support dynamic values of DataType::Num determined by reading user input.
template<class T>
class PerDataSpace : public DynamicArray<T>
{
 public:
  PerDataSpace() :
    DynamicArray<T>(unsigned(DataType::Num))
  {
  }

  PerDataSpace(const T & val) :
    DynamicArray<T>(unsigned(DataType::Num))
  {
    this->fill(val);
  }

  PerDataSpace(std::initializer_list<T> l) :
    DynamicArray<T>(l)
  {
    assert(this->size() == unsigned(DataType::Num));
  }

  T & operator [] (unsigned pv)
  {
    assert(pv < unsigned(DataType::Num));
    return DynamicArray<T>::at(pv);
  }
  const T & operator [] (unsigned pv) const
  {
    assert(pv < unsigned(DataType::Num));
    return DynamicArray<T>::at(pv);
  }

  T & operator [] (DataType pv)
  {
    return (*this)[unsigned(pv)];
  }
  const T & operator [] (DataType pv) const
  {
    return (*this)[unsigned(pv)];
  }

  T & at(DataType pv)
  {
    return (*this)[pv];
  }
  const T & at(DataType pv) const
  {
    return (*this)[pv];
  }

  T & at(unsigned pv)
  {
    return (*this)[pv];
  }
  const T & at(unsigned pv) const
  {
    return (*this)[pv];
  }

  void clear()
  {
    DynamicArray<T>::clear();
  }

  T Max() const
  {
    return *std::max_element(this->begin(), this->end());
  }

  friend std::ostream& operator << (std::ostream& out, const PerDataSpace<T>& x)
  {
    for (unsigned pvi = 0; pvi < unsigned(DataType::Num); pvi++)
    {
      out << std::setw(10) << DataType(pvi) << ": " << x[pvi] << std::endl;
    }
    return out;
  }

  // Serialization
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version = 0)
  {
    if (version == 0)
    {
      ar& boost::serialization::make_nvp(
        "PerDataSpace",
        boost::serialization::make_array(this->begin(), this->size()));
    }
  }
};

template<class T>
std::ostream& operator<<(std::ostream& out, const PerDataSpace<T>& px);

enum class WeightDimension {
  R,
  S,
  C,
  K,
  Num
};
enum class InputDimension {
  W,
  H,
  C,
  N,
  Num
};
enum class OutputDimension {
  P,
  Q,
  K,
  N,
  Num
};
enum class Dimension {
  R,
  S,
  P,
  Q,
  C,
  K,
  N,
  Num
};

extern std::map<Dimension, std::string> DimensionName;
extern std::map<char, Dimension> DimensionID;

std::ostream& operator << (std::ostream& out, const Dimension& dim);

// Think of this as std::array<T, Dimension::Num>, except that the goal is
// to support dynamic values of Dimension::Num determined by reading user input.
template<class T>
class PerProblemDimension : public DynamicArray<T>
{
 public:
  PerProblemDimension() :
    DynamicArray<T>(unsigned(Dimension::Num))
  {
  }

  PerProblemDimension(std::initializer_list<T> l) :
    DynamicArray<T>(l)
  {
    assert(this->size() == unsigned(Dimension::Num));
  }

  friend std::ostream& operator << (std::ostream& out, const PerProblemDimension<T>& x)
  {
    for (unsigned i = 0; i < x.size(); i++)
    {
      out << Dimension(i) << ": " << x[i] << std::endl;
    }
    return out;
  }

  // Serialization
  friend class boost::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, const unsigned int version = 0)
  {
    if (version == 0)
    {
      ar << boost::serialization::make_nvp(
        "PerProblemDimension",
        boost::serialization::make_array(this->begin(), this->size()));
    }
  }
};

template<class T>
std::ostream& operator<<(std::ostream& out, const PerProblemDimension<T>& x);

typedef Point<int(Dimension::Num)> ProblemPoint;

typedef Point<int(WeightDimension::Num)> WeightPoint;
typedef Point<int(InputDimension::Num)> InputPoint;
typedef Point<int(OutputDimension::Num)> OutputPoint;

typedef PointSet<int(WeightDimension::Num)> WeightPointSet;
typedef PointSet<int(InputDimension::Num)> InputPointSet;
typedef PointSet<int(OutputDimension::Num)> OutputPointSet;

typedef std::map<problem::Dimension, int> Bounds;
typedef std::map<problem::DataType, double> Densities;

// ======================================== //
//              WorkloadConfig              //
// ======================================== //

class WorkloadConfig
{
  Bounds bounds_;
  Densities densities_;

  // Stride and dilation. FIXME: ugly.
  int Wstride, Hstride;
  int Wdilation, Hdilation;
  
 public:
  WorkloadConfig() {}

  int getBound(problem::Dimension dim) const
  {
    return bounds_.at(dim);
  }
  
  double getDensity(problem::DataType pv) const
  {
    return densities_.at(pv);
  }

  int getWstride() const { return Wstride; }
  void setWstride(const int s) { Wstride = s; }
  
  int getHstride() const { return Hstride; }
  void setHstride(const int s) { Hstride = s; }

  int getWdilation() const { return Wdilation; }
  void setWdilation(const int s) { Wdilation = s; }

  int getHdilation() const { return Hdilation; }
  void setHdilation(const int s) { Hdilation = s; }

  void setBounds(const std::map<problem::Dimension, int> &bounds)
  {
    bounds_ = bounds;
  }
  
  void setDensities(const std::map<problem::DataType, double> &densities)
  {
    densities_ = densities;
  }

 private:
  // Serialization
  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive& ar, const unsigned int version = 0)
  {
    if (version == 0)
    {
      ar& BOOST_SERIALIZATION_NVP(bounds_);
      ar& BOOST_SERIALIZATION_NVP(densities_);
    }
  }
};

WeightPoint MakeWeightPoint(WorkloadConfig* wc, const ProblemPoint& problem_point);
InputPoint MakeInputPoint(WorkloadConfig* wc, const ProblemPoint& problem_point);
OutputPoint MakeOutputPoint(WorkloadConfig* wc, const ProblemPoint& problem_point);

// ======================================== //
//               AllPointSets               //
// ======================================== //

class AllPointSets
{
 private:
  WorkloadConfig* workload_config_;
  
  WeightPointSet weights_;
  InputPointSet inputs_;
  OutputPointSet outputs_;

 public:
  AllPointSets() : workload_config_(nullptr), weights_(), inputs_(), outputs_() {}
  AllPointSets(WorkloadConfig* wc);
  AllPointSets(WorkloadConfig* wc, const ProblemPoint& low, const ProblemPoint& high);

  void Reset();
  AllPointSets& operator+=(const AllPointSets& s);
  AllPointSets& operator+=(const ProblemPoint& p);
  AllPointSets operator-(const AllPointSets& p);
  PerDataSpace<std::size_t> GetSizes() const;
  std::size_t GetSize(const int t) const;
  bool IsEmpty(const int t) const;
  bool CheckEquality(const AllPointSets& rhs, const int t) const;
  void PrintSizes();
  void Print() const;
  void Print(DataType pv) const;
};

PerDataSpace<std::size_t> GetMaxWorkingSetSizes(
    PerProblemDimension<int> dimension_sizes);

} // namespace problem