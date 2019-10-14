#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <Unknwn.h>
#include <wil\cppwinrt.h>
#include <winrt\base.h>
#include <winrt\windows.system.h>
#include <winrt\windows.ui.core.h>
#include <ProjectTelemetry.h>
#include <TraceLoggingActivity.h>
#include <wil\common.h>
#include <wil\result.h>

#include "lib\trace.h"
#include "lib\FancyZones.h"
#include "lib\Settings.h"

namespace winrt
{
    using namespace ::winrt;
}