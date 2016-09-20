#pragma once

//class UdfFileExtent
//{
//public:
//	int Length;
//
//	int Position;
//
//		public void Parse(int start, byte[] buffer) {
//		this.Length = UdfHelper.Get32(start, buffer);
//		this.Position = UdfHelper.Get32(start + 4, buffer);
//	}
//}
#include "ShortAllocationDescriptor.h"

typedef ShortAllocationDescriptor UdfFileExtent;