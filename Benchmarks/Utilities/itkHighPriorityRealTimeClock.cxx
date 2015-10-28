/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHighPriorityRealTimeClock.cxx,v $
  Language:  C++
  Date:      $Date: 2007/04/12 21:35:26 $
  Version:   $Revision: 1.8 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include "itkHighPriorityRealTimeClock.h"

#if defined(WIN32) || defined(_WIN32)

#else

#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

#endif  // defined(WIN32) || defined(_WIN32)

namespace itk
{

void HighPriorityRealTimeClock::DisplayErrorMessage()
{
#if defined(WIN32) || defined(_WIN32)
  LPVOID lpMsgBuf;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL
    );
  MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
  LocalFree(lpMsgBuf);
#endif  // defined(WIN32) || defined(_WIN32)
}

HighPriorityRealTimeClock::HighPriorityRealTimeClock()
{
  this->RaisePriority();
}

HighPriorityRealTimeClock::~HighPriorityRealTimeClock()
{
  this->RestorePriority();
}

void HighPriorityRealTimeClock::RaisePriority()
{
#if defined(WIN32) || defined(_WIN32)

  this->dwOldPriorityClass = ::GetPriorityClass(::GetCurrentProcess());
  if (!this->dwOldPriorityClass)
    {
    this->DisplayErrorMessage();
    itkExceptionMacro("Current priority class could not be retrieved");
    }
  // REALTIME_PRIORITY_CLASS will pretty much block the mouse, cause
  // some program to lose socket connection, etc.
  else if (!::SetPriorityClass(::GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
    // Note that Administrators will be able to reach REALTIME, whether other
    // users will be capped to HIGH (without error).
    this->DisplayErrorMessage();
    itkExceptionMacro("Priority class could not be set");
    }

  this->nOldThreadPriority = ::GetThreadPriority(::GetCurrentThread());
  if (this->nOldThreadPriority == THREAD_PRIORITY_ERROR_RETURN)
    {
    this->DisplayErrorMessage();
    itkExceptionMacro("Current thread priority could not be retrieved");
    }
  else if (!::SetThreadPriority(::GetCurrentThread(),
                                THREAD_PRIORITY_TIME_CRITICAL))
    {
    this->DisplayErrorMessage();
    itkExceptionMacro("Thread priority could not be set");
    }

#else

  errno = 0;
  this->OldProcessPriority = getpriority(PRIO_PROCESS, 0);
  if (this->OldProcessPriority == -1 && errno != 0)
    {
    itkExceptionMacro("Current process priority could not be retrieved");
    }
  else
    {
    errno = 0;
    // Technically only root can upgrade our priority, i.e. set the priority
    // to such a low value that the system will favor our process for
    // scheduling. In most other cases, we would get an EACCESS.
    int prio;
#ifdef __APPLE__
    prio = -20;
#else
    prio = -NZERO;
#endif
    if (setpriority(PRIO_PROCESS, 0, prio) == -1 &&
        errno != EACCES &&
        errno != EPERM)
      {
      std::string msg("Process priority could not be set: ");
      switch (errno)
        {
        case EACCES:
          msg += "EACCES";
          break;
        case EINVAL:
          msg += "EINVAL";
          break;
        case ENOTSUP:
          msg += "ENOTSUP";
          break;
        case EPERM:
          msg += "EPERM";
          break;
        case ESRCH:
          msg += "ESRCH";
          break;
        default:
          msg += "Unknown";
          break;
        }
      itkExceptionMacro(<< msg.c_str());
      }
    }

#endif  // defined(WIN32) || defined(_WIN32)
}

void HighPriorityRealTimeClock::RestorePriority()
{
#if defined(WIN32) || defined(_WIN32)

  if (this->dwOldPriorityClass &&
      !::SetPriorityClass(::GetCurrentProcess(), this->dwOldPriorityClass))
    {
    this->DisplayErrorMessage();
    }

  if (this->nOldThreadPriority != THREAD_PRIORITY_ERROR_RETURN &&
      !::SetThreadPriority(::GetCurrentThread(), this->nOldThreadPriority))
    {
    this->DisplayErrorMessage();
    }

#else

  errno = 0;
  if (setpriority(PRIO_PROCESS, 0, this->OldProcessPriority) == -1)
    {
    std::string msg("Process priority could not be set: ");
    switch (errno)
      {
      case EACCES:
        msg += "EACCES";
        break;
      case EINVAL:
        msg += "EINVAL";
        break;
      case ENOTSUP:
        msg += "ENOTSUP";
        break;
      case EPERM:
        msg += "EPERM";
        break;
      case ESRCH:
        msg += "ESRCH";
        break;
      default:
        msg += "Unknown";
        break;
      }
    itkExceptionMacro(<< msg.c_str());
    }

#endif  // defined(WIN32) || defined(_WIN32)
}

}
