/****************************************************************
*
* Copyright (c) 2010
*
* Fraunhofer Institute for Manufacturing Engineering
* and Automation (IPA)
*
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Project name: care-o-bot
* ROS stack name: cob_driver
* ROS package name: cob_camera_sensors
* Description: Virtual range camera representation.
*
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Author: Jan Fischer, email:jan.fischer@ipa.fhg.de
* Supervised by: Jan Fischer, email:jan.fischer@ipa.fhg.de
*
* Date of creation: Jan 2009
* ToDo:
*
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* * Neither the name of the Fraunhofer Institute for Manufacturing
* Engineering and Automation (IPA) nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License LGPL as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License LGPL for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License LGPL along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************/

/// @file VirtualRangeCam.h
/// Virtual range camera representation.
/// @author Jan Fischer 
/// @date 2009

#ifndef __IPA_VIRTUALRANGECAM_H__
#define __IPA_VIRTUALRANGECAM_H__

#ifdef __LINUX__
	#include <cob_camera_sensors/AbstractRangeImagingSensor.h>
#else
	#include <cob_driver/cob_camera_sensors/common/include/cob_camera_sensors/AbstractRangeImagingSensor.h>
#endif

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sstream>

#include <boost/filesystem.hpp>

#ifdef SWIG
%module Sensors3D

%{
	#include "Swissranger.h"
%}
#endif

namespace fs = boost::filesystem;
using namespace ipa_Utils;

namespace ipa_CameraSensors {

static const int SWISSRANGER_COLUMNS = 176;
static const int SWISSRANGER_ROWS = 144;

/// @ingroup VirtualCameraDriver
/// Interface class to virtual range camera like Swissranger 3000/4000.
/// The class offers an interface to a virtual range camera, that is equal to the interface of a real range camera.
/// However, pictures are read from a directory instead of the camera.
class __DLL_LIBCAMERASENSORS__ VirtualRangeCam : public AbstractRangeImagingSensor
{
public:

	VirtualRangeCam();
	~VirtualRangeCam();

	//*******************************************************************************
	// AbstractRangeImagingSensor interface implementation
	//*******************************************************************************

	unsigned long Init(std::string directory, int cameraIndex = 0);

	unsigned long Open();
	unsigned long Close();

	unsigned long SetProperty(t_cameraProperty* cameraProperty);
	unsigned long SetPropertyDefaults();
	unsigned long GetProperty(t_cameraProperty* cameraProperty);

	unsigned long AcquireImages(int widthStepRange, int widthStepGray, int widthStepCartesian, char* rangeImage=NULL, char* intensityImage=NULL,
		char* cartesianImage=NULL, bool getLatestFrame=true, bool undistort=true,
		ipa_CameraSensors::t_ToFGrayImageType grayImageType = ipa_CameraSensors::INTENSITY_32F1);
	unsigned long AcquireImages(cv::Mat* rangeImage = 0, cv::Mat* intensityImage = 0,
		cv::Mat* cartesianImage = 0, bool getLatestFrame = true, bool undistort = true,
		ipa_CameraSensors::t_ToFGrayImageType grayImageType = ipa_CameraSensors::INTENSITY_32F1);

	unsigned long GetCalibratedUV(double x, double y, double z, double& u, double& v);

	unsigned long SaveParameters(const char* filename);

	bool isInitialized() {return m_initialized;}
	bool isOpen() {return m_open;}

	/// Returns the number of images in the directory
	/// @return The number of images in the directory
	int GetNumberOfImages();

	/// Function specific to virtual camera.
	/// Resets the image directory read from the configuration file.
	/// @param path The camera path
	/// @return Return code
	unsigned long SetPathToImages(std::string path);

private:
	//*******************************************************************************
	// Camera specific members
	//*******************************************************************************

	unsigned long GetCalibratedZMatlab(int u, int v, float zRaw, float& zCalibrated);
	unsigned long GetCalibratedXYMatlab(int u, int v, float z, float& x, float& y);
	
	/// Load general range camera parameters .
	/// @param filename Configuration file-path and file-name.
	/// @param cameraIndex The index of the camera within the configuration file
	///		   i.e. SR_CAM_0 or SR_CAM_1
	/// @return Return code
	unsigned long LoadParameters(const char* filename, int cameraIndex);

	bool m_CoeffsInitialized;

	/// Given a 32 bit swissranger depth value, the real depth value in meteres is given by:
	/// z(u,v)=a0(u,v)+a1(u,v)*d(u,v)+a2(u,v)*d(u,v)^2
	///       +a3(u,v)*d(u,v)^3+a4(u,v)*d(u,v)^4+a5(u,v)*d(u,v)^5
	///       +a6(u,v)*d(u,v)^6;
	cv::Mat m_CoeffsA0; ///< a0 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA1; ///< a1 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA2; ///< a2 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA3; ///< a3 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA4; ///< a4 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA5; ///< a5 z-calibration parameters. One matrix entry corresponds to one pixel
	cv::Mat m_CoeffsA6; ///< a6 z-calibration parameters. One matrix entry corresponds to one pixel

	std::string m_CameraDataDirectory; ///< Directory where the image data resides
	int m_CameraIndex; ///< Index of the specified camera. Important, when several cameras of the same type are present

	std::vector<std::string> m_AmplitudeImageFileNames;
	std::vector<std::string> m_IntensityImageFileNames;
	std::vector<std::string> m_RangeImageFileNames ;
	std::vector<std::string> m_CoordinateImageFileNames ;

	int m_ImageWidth;  ///< Image width
	int m_ImageHeight; ///< Image height

	double m_k1, m_k2, m_p1, m_p2; ///< Distortion parameters
};

/// Creates, intializes and returns a smart pointer object for the camera.
/// @return Smart pointer, refering to the generated object
__DLL_LIBCAMERASENSORS__ AbstractRangeImagingSensorPtr CreateRangeImagingSensor_VirtualCam();

} // end namespace ipa_CameraSensors
#endif // __IPA_VIRTUALRANGECAM_H__


