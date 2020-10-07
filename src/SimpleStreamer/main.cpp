#include <iostream>
#include <cstring>
#include <cstdlib>

#include "ueye.h"
#include "ueye_stream.h"

void abortOnError(const int status, const char *msg)
{
    if (status != 0)
    {
        std::cout << msg << status << std::endl;
        exit(status);
    }
}

int main()
{
    /* initialize first available camera */
    HIDS hCam = 0;
    int nRet = is_InitCamera(&hCam, 0);
    abortOnError(nRet, "InitCamera failed with error code: ");

    /* receive sensor info */
    SENSORINFO sensorInfo;
    nRet = is_GetSensorInfo(hCam, &sensorInfo);
    abortOnError(nRet, "Receving sensor info failed with error code: ");

    /* receive camera image size */
    IS_RECT rectAoi;
    nRet = is_AOI(hCam, IS_AOI_IMAGE_GET_AOI, &rectAoi, sizeof(rectAoi));
    abortOnError(nRet, "Receving image aoi failed with error code: ");

    /* set color mode */
    nRet = is_SetColorMode(hCam, IS_CM_BGR8_PACKED);
    abortOnError(nRet, "Setting color mode failed with error code: ");

    /* allocate image buffer */
    char *pMemoryBuffer;
    int nMemoryId;
    nRet = is_AllocImageMem(hCam, rectAoi.s32Width, rectAoi.s32Height, 24, &pMemoryBuffer, &nMemoryId);
    abortOnError(nRet, "Allocating image memory failed with error code: ");

    /* set allocated image buffer active */
    nRet = is_SetImageMem(hCam, pMemoryBuffer, nMemoryId);
    abortOnError(nRet, "Setting image memory active failed with error code: ");

    /* Stream setup sequence
     * Step 1: Initialize streaming library
     * Step 2: Add a session
     * Step 3: Add a video stream to the session
     * Step 4: Submit frame data to the stream
     */

    /* Step 1: Initialize streaming library */
    nRet = is_Stream(IS_STREAM_INIT, 0, 0);
    abortOnError(nRet, "Initializing streaming server failed with error code: ");

    /* Step 2: Add a session  */
    IS_SESSION_INFO sessionInfo;
    sessionInfo.cbSizeOfStruct = sizeof(IS_SESSION_INFO);
    strcpy(sessionInfo.strDescription, "Simple ueye stream");
    strcpy(sessionInfo.strInfo, sensorInfo.strSensorName);
    strcpy(sessionInfo.strName, "ueye_stream");

    nRet = is_Stream(IS_STREAM_ADD_SESSION, (void*)&sessionInfo, sizeof(sessionInfo));
    abortOnError(nRet, "Adding session failed with error code: ");

    /* Step 3: Add a video stream to the session */

    /* setup video input and encode options */
    IS_STREAM_VIDEO_CONFIGURATION videoConfiguration;
    videoConfiguration.cbSizeOfStruct = sizeof(IS_STREAM_VIDEO_CONFIGURATION);

    videoConfiguration.framerate = 10;
    videoConfiguration.bitrate = 500000;

    /* setup input configuration */
    videoConfiguration.srcCodec = IS_STREAM_CODEC_NONE;
    videoConfiguration.srcHeight = rectAoi.s32Height;
    videoConfiguration.srcWidth = rectAoi.s32Width;
    videoConfiguration.srcPixelformat = IS_STREAM_FMT_BGR24;

    /* setup ouput configuration */
    videoConfiguration.dstHeight = rectAoi.s32Height / 2;
    videoConfiguration.dstWidth = rectAoi.s32Width / 2;
    videoConfiguration.dstCodec = IS_STREAM_CODEC_MJPEG;

    /* add a video stream to the previously added session */
    IS_STREAM_INFO streamInfo;
    streamInfo.cbSizeOfStruct = sizeof(IS_STREAM_INFO);
    streamInfo.idSession = sessionInfo.idSession;

    streamInfo.streamType = IS_STREAM_TYPE_VIDEO;
    streamInfo.pStreamData = &videoConfiguration;

    nRet = is_Stream(IS_STREAM_ADD_STREAM, (void*)&streamInfo, sizeof(streamInfo));
    abortOnError(nRet, "Adding stream failed with error code: ");

    std::cout << "Stream Access: " << sessionInfo.strURL << std::endl;

    /* capture a single image and submit it to the streaming library */
    for (;;)
    {
        /* capture a single frame */
        nRet = is_FreezeVideo(hCam, IS_WAIT);

        /* Step 4: Submit frame data to the stream */
        IS_STREAM_PAYLOAD_DATA data;
        data.cbSizeOfStruct = sizeof(IS_STREAM_PAYLOAD_DATA);
        data.pData = reinterpret_cast<unsigned char*>(pMemoryBuffer);
        data.cbSizeOfData = rectAoi.s32Width * rectAoi.s32Height * 3;
        data.idSession = sessionInfo.idSession;
        data.idStream = streamInfo.idStream;

        /* submit image frame */
        nRet = is_Stream(IS_STREAM_SUBMIT_DATA, (void*)&data, sizeof(IS_STREAM_PAYLOAD_DATA));
    }

    /* uninitialize streaming server */
    nRet = is_Stream(IS_STREAM_EXIT, 0, 0);
    abortOnError(nRet, "Uninitializing streaming server failed with error code: ");

    /* close camera */
    nRet = is_ExitCamera(hCam);
    abortOnError(nRet, "exit camera failed with error code: ");

    return 0;
}
