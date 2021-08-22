//Confidential and proprietary to Innoviz Technologies Ltd//

#ifndef __IWRITER_H__
#define __IWRITER_H__

/**
 * @file IWriter.h
 */

#include "../common_includes/invz_types.h"
#include "../common_includes/invz_status.h"
#include "../common_includes/invz_constants.h"

namespace invz {
	
	class INVZ_API IWriter
	{
	public:
		/* New fame meta */
		virtual Result DumpFrameToFile(	CSampleFrameMeta frame_meta, 
										MacroPixelFixed* macro_pixel_list, 
										size_t num_of_macro_pixels, 
										EnvironmentalBlockage* environmental_blockage = nullptr, 
										BlockageDetectionSegment* blockage_detection = nullptr,
										uint8_t* blockage_classifications =  nullptr, 
										size_t num_of_blockage_segments = 0) = 0;

	};




}

#endif /*__IWRITER_H__*/