/*! \file    ueye_stream.h
*   \author  (c) 20013 - 2016 by Imaging Development Systems GmbH
*   \date    Date: 2016/05/13
*
*   \brief   Library interface for ueye stream
*/

/*! \mainpage ueye_stream
 *
 * \section intro_sec Introduction
 *
 * ueye stream is a library for a standard RTP multimedia stream. This library can be used to build streaming applications.
 * The library can be used to stream MJPEG and H.264 video.
 *
 * \section install_sec How to use the library
 *
 * \subsection step1 Step 1: Initialize streaming library and create rtsp server
 * \code
 *
 * unsigned int port = 1554;
 * is_Stream(IS_STREAM_INIT, (void*)&port, sizeof(port));
 *
 * \endcode
 * \subsection step2 Step 2: Add a session
 * \code
 *
 *  IS_SESSION_INFO sessionInfo;
 *  sessionInfo.cbSizeOfStruct = sizeof(IS_SESSION_INFO);
 *  strcpy(sessionInfo.strDescription, "Simple ueye stream");
 *  strcpy(sessionInfo.strInfo, sensorInfo.strSensorName);
 *  strcpy(sessionInfo.strName, "ueye_stream");
 *
 *  nRet = is_Stream(IS_STREAM_ADD_SESSION, (void*)&sessionInfo, sizeof(sessionInfo));
 *
 * \endcode
 * \subsection step3 Step 3: Add a video stream to the session
 * \code
 *
 * IS_STREAM_VIDEO_CONFIGURATION videoConfiguration;
 * videoConfiguration.cbSizeOfStruct = sizeof(IS_STREAM_VIDEO_CONFIGURATION);
 *
 * videoConfiguration.framerate = 10;
 * videoConfiguration.bitrate = 500000;
 *
 *
 * videoConfiguration.srcCodec = IS_STREAM_CODEC_NONE;
 * videoConfiguration.srcHeight = rectAoi.s32Height;
 * videoConfiguration.srcWidth = rectAoi.s32Width;
 * videoConfiguration.srcPixelformat = IS_STREAM_FMT_BGR24;
 *
 *
 * videoConfiguration.dstHeight = rectAoi.s32Height / 2;
 * videoConfiguration.dstWidth = rectAoi.s32Width / 2;
 * videoConfiguration.dstCodec = IS_STREAM_CODEC_MJPEG;
 *
 *
 * IS_STREAM_INFO streamInfo;
 * streamInfo.cbSizeOfStruct = sizeof(IS_STREAM_INFO);
 * streamInfo.idSession = sessionInfo.idSession;
 *
 * streamInfo.streamType = IS_STREAM_TYPE_VIDEO;
 * streamInfo.pStreamData = &videoConfiguration;
 *
 * nRet = is_Stream(IS_STREAM_ADD_STREAM, (void*)&streamInfo, sizeof(streamInfo));
 *
 * \endcode
 * \subsection step4 Step 4: Submit frame data to the stream
 * \code
 *
 * IS_STREAM_PAYLOAD_DATA streamData;
 * streamData.cbSizeOfStruct = sizeof(IS_STREAM_PAYLOAD_DATA);
 * streamData.pData = streamData;
 * streamData.cbSizeOfData = sizeOfStreamData;
 * streamData.idSession = sessionInfo.idSession;
 * streamData.idStream = streamInfo.idStream;
 *
 * nRet = is_Stream(IS_STREAM_SUBMIT_DATA, (void*)&streamData, sizeof(streamData));
 * \endcode
 */

#ifndef UEYE_STREAM_H_
#define UEYE_STREAM_H_

#if defined (_MSC_VER)
#if defined UEYE_STREAM_LIB_EXPORT
#define UEYE_STREAM_API __declspec(dllexport)
#else
#define UEYE_STREAM_API __declspec(dllimport)
#endif
#elif defined (__GNUC__)
#if defined UEYE_STREAM_LIB_EXPORT
#define UEYE_STREAM_API __attribute__ ((visibility ("default")))
#else
#define UEYE_STREAM_API
#endif
#else
#define UEYE_STREAM_API
#endif

typedef void* IS_STREAM_ID;

/* error codes */
#define IS_STREAM_NO_SUCCESS -1
#define IS_STREAM_SUCCESS 0
#define IS_STREAM_NOT_INITIALIZED 1
#define IS_STREAM_ALREADY_INITIALIZED 2
#define IS_STREAM_INVALID_PARAMETER 3
#define IS_STREAM_NAME_IN_USE 4
#define IS_STREAM_INVALID_ID 5
#define IS_STREAM_NO_MEMORY 6
#define IS_STREAM_ENCODER_ERROR 7

/*!
 * \brief Enumeration of various pixel formats.
 */
typedef enum E_STREAM_PIXELFMT
{
    /*! \brief Unkown pixel format, library will not accept this pixel format */
    IS_STREAM_FMT_NONE = -1,
    /*! \brief RGBA (8 8 8 8) occupies 32 bits */
    IS_STREAM_FMT_RGBA = 28,
    /*! \brief BGRA (8 8 8 8) occupies 32 bits */
    IS_STREAM_FMT_BGRA = 30,
    /*! \brief RGB (8 8 8), occupies 24 bits */
    IS_STREAM_FMT_RGB24 = 2,
    /*! \brief BGR (8 8 8), occupies 24 bits */
    IS_STREAM_FMT_BGR24 = 3

} IS_STREAM_PIXELFMT;

/*!
 * \brief Enumeration of various codec types.
 */
typedef enum E_STREAM_CODEC
{
    /*! \brief Use this if incoming data has no codec*/
    IS_STREAM_CODEC_NONE = 0,
    /*! \brief Library will stream in H264 */
    IS_STREAM_CODEC_H264 = 28,
    /*! \brief Library will stream in motion jpeg */
    IS_STREAM_CODEC_MJPEG = 8

} IS_STREAM_CODEC;

/*!
 * \brief Enumeration of various stream types.
 */
typedef enum E_STREAM_TYPES
{
    /*! \brief Streaming type is video */
    IS_STREAM_TYPE_VIDEO = 0,

    /*! \brief Streaming type is audio (not supported) */
    IS_STREAM_TYPE_AUDIO = 1

} IS_STREAM_TYPES;

/*!
 * \brief Enumeration of commands supported by the stream access function, \see is_Stream.
 */
typedef enum E_STREAM_COMMAND
{
    /*! \brief Initialize the streaming interface */
    IS_STREAM_INIT,

    /*! \brief Deinitialize the streaming interface. */
    IS_STREAM_EXIT,

    /*! \brief Add a new session. */
    IS_STREAM_ADD_SESSION,

    /*! \brief Remove a previously added session. */
    IS_STREAM_REMOVE_SESSION,

    /*! \brief Add a stream to a previously added session. */
    IS_STREAM_ADD_STREAM,

    /*! \brief Submit data to the stream. */
    IS_STREAM_SUBMIT_DATA,

    /*! \brief Receive information about the session. */
    IS_STREAM_GET_SESSION_INFO

} IS_STREAM_COMMAND;


/*! \brief Session information struct.
 * The session information struct is used to add a new session or to receive information about a previously added session.
 */
typedef struct
{
    /*! \brief Size of the struct */
    int cbSizeOfStruct;

    /*! \brief Session identifier */
    IS_STREAM_ID idSession;

    /*! \brief Name of the stream */
    char strName[20];
    /*! \brief Information about the stream */
    char strInfo[20];
    /*! \brief Description about the stream */
    char strDescription[20];
    /*! \brief Stream URL will be filled automaticly */
    char strURL[255];

} IS_SESSION_INFO;

/*! \brief Stream information struct
 * The stream information struct is used to add a new stream to a previously added session.
 */
typedef struct
{
    /*! \brief Size of the struct */
    int cbSizeOfStruct;

    /*! \brief Session identifier */
    IS_STREAM_ID idSession;
    /*! \brief Stream identifier */
    IS_STREAM_ID idStream;

    /*! \brief Stream type \see IS_STREAM_TYPES */
    IS_STREAM_TYPES streamType;

    /*! \brief Pointer to specify the stream configuration data */
    void *pStreamData;

} IS_STREAM_INFO;

/*! \brief Video stream configuration
 * Specifies the streaming input and output video configuration.
 */
typedef struct
{
    /*! \brief Size of the struct */
    int cbSizeOfStruct;

    /*! \brief Source video frame width */
    int srcWidth;
    /*! \brief Source video frame height */
    int srcHeight;

    /*! \brief Destination video frame width */
    int dstWidth;
    /*! \brief Destination video frame dith */
    int dstHeight;

    /*! \brief Source pixel format */
    IS_STREAM_PIXELFMT srcPixelformat;
    /*! \brief Not implemented yet */
    IS_STREAM_PIXELFMT dstPixelformat;

    /*! \brief Source codec */
    IS_STREAM_CODEC srcCodec;
    /*! \brief Not implemented yet */
    IS_STREAM_CODEC dstCodec;

    /*! \brief Expected frames per second */
    int framerate;
    /*! \brief Bitrate in bits*/
    unsigned int bitrate;

} IS_STREAM_VIDEO_CONFIGURATION;

/*! \brief Struct to deliver a substream data
 *  Specifies the streaming input data.
 */
typedef struct
{
    /*! \brief Size of the struct */
    int cbSizeOfStruct;

    /*! \brief Identifier to the session */
    IS_STREAM_ID idSession;
    /*! \brief Identifier to the stream */
    IS_STREAM_ID idStream;

    /*! \brief Pointer to data to submit */
    unsigned char* pData;
    /*! \brief Size of data to submit */
    int cbSizeOfData;

} IS_STREAM_PAYLOAD_DATA;


/*!
 * \brief  Generic interface to setup and create a stream.
 * \param  nCommand        code, indicates requested access and accessed value, \see IS_STREAM_COMMAND enumeration.
 * \param  pParam          input or output storage for the accessed param.
 * \param  cbSizeOfParam   size of *pParam.
 * \return error code
 */
UEYE_STREAM_API int is_Stream(int nCommand, void* pParam, int cbSizeOfParam);



#endif /* UEYE_STREAM_H_ */
