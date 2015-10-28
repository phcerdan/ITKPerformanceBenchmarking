/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkHighPriorityRealTimeProbe_h
#define itkHighPriorityRealTimeProbe_h

#include "itkConfigure.h"
//#include "itkResourceProbe.h"
//#include "itkRealTimeClock.h"
#include "itkMacro.h"
#include "itkIntTypes.h"
#include <string>
#include "itkHighPriorityRealTimeClock.h"

namespace itk
{
/** \class HighPriorityRealTimeProbe
 *
 *  \brief Computes the time passed between two points in code.
 *
 *   This class allows the user to trace the time passed between the execution
 *   of two pieces of code.  It can be started and stopped in order to evaluate
 *   the execution over multiple passes.  The values of time are taken from the
 *   HighPriorityRealTimeClock.
 *
 *   \sa HighPriorityRealTimeClock
 *
 */
class ITKCommon_EXPORT HighPriorityRealTimeProbe
{
public:

  /** Type for counting how many times the probe has been started and stopped.
    */
  typedef unsigned long CountType;

  /** Type for measuring time. See the RealTimeClock class for details on the
   * precision and units of this clock signal */
  typedef RealTimeClock::TimeStampType TimeStampType;

public:

  /** Constructor */
  HighPriorityRealTimeProbe();

  /** Destructor */
  virtual ~HighPriorityRealTimeProbe();

  /** Set name of target */
  void SetNameOfTarget(std::string name);

  /** Reset */
  void Reset();

  /** Reset the probe */
  void ResetProbeData();

  /** Returns the type probed value */
  std::string GetType() const;

  /** Returns the unit probed value */
  std::string GetUnit() const;

  /** Get the current time.
   *  Warning: the returned value is not the elapsed time since the last Start() call.
   */
  virtual TimeStampType GetInstantValue() const;

  /** Start counting the change of value */
  void Start();

  /** Stop counting the change of value.
   *
   * If a matching Start() has not been called before, there is no
   * effect.
   **/
  void Stop();

  /** Compute mean, and standard deviation of measured computation time */
  bool Evaluate();

  /** Returns the number of times that the probe has been started */
  CountType GetNumberOfStarts() const;

  /** Returns the number of times that the probe has been stopped */
  CountType GetNumberOfStops() const;

  /** Returns the accumulated value changes between the starts and stops
   *  of the probe */
  virtual TimeStampType GetTotal() const;

  /** Returns the average value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetMean() const;

  /** Returns the standard deviation value changes between the starts and stops
   *  of the probe. Evaluate() should be called prior to this function
   */
  virtual TimeStampType GetStandardDeviation() const;

  /** Print Probe Results. */
  void PrintProbeResults();

  void TestPut(TimeStampType value);

  /** Get a handle to m_RealTimeClock. */
  itkGetConstObjectMacro( HighPriorityRealTimeClock, HighPriorityRealTimeClock );


protected:
  /** Update the Min and Max values with an input value */
  void UpdatekMinMaxValue(TimeStampType& value);

  /** Check validation of measurements*/
  bool CheckValidation();

private:
  std::string                        m_NameOfTargetClass;
  std::string                        m_TypeString;
  std::string                        m_UnitString;

  TimeStampType                      m_StartValue;
  TimeStampType                      m_TotalValue;
  TimeStampType                      m_MinValue;
  TimeStampType                      m_MaxValue;
  TimeStampType                      m_MeanValue;
  TimeStampType                      m_StandardDeviation;

  CountType                          m_NumberOfStarts;
  CountType                          m_NumberOfStops;
  CountType                          m_NumberOfIteration;

  HighPriorityRealTimeClock::Pointer m_HighPriorityRealTimeClock;
  std::vector<TimeStampType>         m_ElapsedTimeList;

};
} // end namespace itk

#endif //itkHighPriorityRealTimeProbe_h
