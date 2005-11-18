/* Alib.h, /audio/alib/include, sung, 06/26/91

(c) Copyright 1983, 1984, 1985, 1986, 1987, 1988 Hewlett-Packard Company.
(c) Copyright 1979 The Regents of the University of Colorado, a body corporate 
(c) Copyright 1979, 1980, 1983 The Regents of the University of California
(c) Copyright 1980, 1984, 1986 AT&T Technologies.  All Rights Reserved.
The contents of this software are proprietary and confidential to the Hewlett-
Packard Company, and are limited in distribution to those with a direct need
to know.  Individuals having access to this software are responsible for main-
taining the confidentiality of the content and for keeping the software secure
when not in use.  Transfer to any party is strictly forbidden other than as
expressly permitted in writing by Hewlett-Packard Company.  Unauthorized trans-
fer to or possession by any unauthorized party may be a criminal offense.

                    RESTRICTED RIGHTS LEGEND

          Use,  duplication,  or disclosure by the Government  is
          subject to restrictions as set forth in subdivision (b)
          (3)  (ii)  of the Rights in Technical Data and Computer
          Software clause at 52.227-7013.

                     HEWLETT-PACKARD COMPANY
                        3000 Hanover St.
                      Palo Alto, CA  94304
*/

#ifndef _ALIB_H_
#define _ALIB_H_

#ifndef NeedFunctionPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define NeedFunctionPrototypes  1
#else
#define NeedFunctionPrototypes  0
#endif /* __STDC__ */
#endif /* NeedFunctionPrototypes */

#include <sys/types.h>
#include <netinet/in.h>
/************ #include <idl/c/idl_base.h>  ************ Praat **************/

#ifndef FILE
#include <stdio.h>
#endif /* FILE */

#ifdef __cplusplus              /* do not leave open across includes */
extern "C" {                            /* for C++ V2.0 */
#endif

#ifndef Bool
#define Bool                    int
#endif

#ifndef True
#define True                    1
#endif

#ifndef False
#define False                   0
#endif

/*
 * Macros to access Audio structure.
 */

#define aConnectionNumber( a )          ( a->fd )
#define aProtocolVersion( a )           ( a->proto_major_version )
#define aProtocolRevision( a )          ( a->proto_minor_version )
#define aServerVendor( a )              ( a->vendor )
#define aVendorRelease( a )             ( a->release )
#define aAudioString( a )               ( a->audio_name )
#define aSoundByteOrder( a )            ( a->byte_order )
#define aSoundBitOrder( a )             ( a->sound_bit_order )
#define aNumDataFormats( a )            ( a->n_data_format )
#define aDataFormats( a )               ( a->data_format_list )
#define aNumSamplingRates( a )          ( a->n_sampling_rate )
#define aSamplingRates( a )             ( a->sampling_rate_list )
#define aBestAudioAttributes( a )       ( a->best_audio_attr )
#define aQLength( a )                   ( a->qlen )
#define aInputSources( a )              ( a->input_sources )
#define aInputChannels( a )             ( a->input_channels )
#define aOutputChannels( a )            ( a->output_channels )
#define aOutputDestinations( a )        ( a->output_destinations )
#define aMaxInputGain( a )              ( a->max_input_gain )
#define aMinInputGain( a )              ( a->min_input_gain )
#define aMaxOutputGain( a )             ( a->max_output_gain )
#define aMinOutputGain( a )             ( a->min_output_gain )
#define aSimplePlayer( a )              ( a->simple_player_gm )
#define aSimpleRecorder( a )            ( a->simple_recorder_gm )
#define aGMGainRestricted( a )          ( a->gm_gain_restricted )

/*
 * Enumerated types for byte and bit order of audio data.
 */

typedef enum _AByteOrder {
    ALSBFirst,
    AMSBFirst
} AByteOrder;

typedef enum _ABitOrder {
    ALeastSignificant,
    AMostSignificant
} ABitOrder;

/*
 * Enumerated type for audio data format.
 */

typedef enum _ADataFormat {
    ADFUnknown,
    ADFMuLaw,
    ADFALaw,
    ADFLin16,
    ADFLin8,
    ADFLin8Offset
/* REMEMBER: add new format name strings to AOpenAudio.c */
} ADataFormat;

/*
 * Type and masks for Input sources.
 */

typedef enum _AInputSrcType {
    AISTMonoMicrophone,
    AISTMonoAuxiliary,
    AISTLeftMicrophone,
    AISTRightMicrophone, 
    AISTLeftAuxiliary,
    AISTRightAuxiliary,
    AISTDefaultInput
} AInputSrcType;

typedef long                    AInputSrcMask;

#define AMonoMicrophoneMask     ( 1 << AISTMonoMicrophone )
#define AMonoAuxiliaryMask      ( 1 << AISTMonoAuxiliary )
#define ALeftMicrophoneMask     ( 1 << AISTLeftMicrophone )
#define ARightMicrophoneMask    ( 1 << AISTRightMicrophone )
#define ALeftAuxiliaryMask      ( 1 << AISTLeftAuxiliary )
#define ARightAuxiliaryMask     ( 1 << AISTRightAuxiliary )
#define ADefaultInputMask       ( 1 << AISTDefaultInput )

/*
 * Type and masks for Input channels.
 */

typedef enum _AInputChType {
    AICTMono,
    AICTLeft,
    AICTRight
} AInputChType;

typedef long                    AInputChMask;

#define AMonoInputChMask        ( 1 << AICTMono )
#define ALeftInputChMask        ( 1 << AICTLeft )
#define ARightInputChMask       ( 1 << AICTRight )

/*
 * Type and masks for Output channels.
 */

typedef enum _AOutputChType {
    AOCTMono,
    AOCTLeft,
    AOCTRight
} AOutputChType;

typedef long                    AOutputChMask;

#define AMonoOutputChMask       ( 1 << AOCTMono )
#define ALeftOutputChMask       ( 1 << AOCTLeft )
#define ARightOutputChMask       ( 1 << AOCTRight )

/*
 * Type and masks for Output destinations.
 */

typedef enum _AOutputDstType {
    AODTMonoIntSpeaker,
    AODTMonoJack,
    AODTLeftIntSpeaker,
    AODTRightIntSpeaker,
    AODTLeftJack,
    AODTRightJack,
    AODTDefaultOutput,
    AODTMonoLineOut,
    AODTLeftLineOut,
    AODTRightLineOut,
    AODTMonoHeadphone,
    AODTLeftHeadphone,
    AODTRightHeadphone
} AOutputDstType;

typedef long                    AOutputDstMask;

#define AMonoIntSpeakerMask     ( 1 << AODTMonoIntSpeaker )
#define AMonoJackMask           ( 1 << AODTMonoJack )
#define ALeftIntSpeakerMask     ( 1 << AODTLeftIntSpeaker )
#define ARightIntSpeakerMask    ( 1 << AODTRightIntSpeaker )
#define ALeftJackMask           ( 1 << AODTLeftJack )
#define ARightJackMask          ( 1 << AODTRightJack )
#define ADefaultOutputMask      ( 1 << AODTDefaultOutput )
#define AMonoLineOutMask        ( 1 << AODTMonoLineOut )
#define ALeftLineOutMask        ( 1 << AODTLeftLineOut )
#define ARightLineOutMask       ( 1 << AODTRightLineOut )
#define AMonoHeadphoneMask      ( 1 << AODTMonoHeadphone )
#define ALeftHeadphoneMask      ( 1 << AODTLeftHeadphone )
#define ARightHeadphoneMask     ( 1 << AODTRightHeadphone )

/*
 * Type for generic channels
 */
typedef enum _AChType {
    ACTMono, ACTLeft, ACTRight
} AChType;

/*
 * Enumerated type for audio file formats.
 */

typedef enum _AFileFormat {
    AFFUnknown,
    AFFRiffWave,
    AFFSun,
    AFFReserved,
    AFFRawMuLaw,
    AFFRawALaw,
    AFFRawLin16,
    AFFRawLin8,
    AFFRawLin8Offset
/* REMEMBER: add new format name strings to AOpenAudio.c */
} AFileFormat;

/*
 * Enumerated type and structure for time specification.
 */

typedef enum _ATimeType {
    ATTSamples,
    ATTMilliSeconds,
    ATTFullLength
} ATimeType;

typedef struct _ATime {
    ATimeType                   type;
    union {
        unsigned long           samples;
        unsigned long           milliseconds;
    } u;
} ATime;

/*
 * Enumerated type and structures for sound bucket and sound streams.
 */

typedef enum _AudioType {
    ATSampled
} AudioType;

typedef struct _ASampledAttributes {
    ADataFormat                 data_format;
    unsigned long               bits_per_sample;
    unsigned long               sampling_rate;
    unsigned long               channels;
    unsigned long               interleave;
    ATime                       duration;
} ASampledAttributes;

typedef enum _ASampledAttributesFields {
    ASAFDataFormat,
    ASAFBitsPerSample,
    ASAFSamplingRate,
    ASAFChannels,
    ASAFInterleave,
    ASAFDuration
} ASampledAttributesFields;

typedef struct _AudioAttributes {
    AudioType                   type;
    union {
        ASampledAttributes      sampled_attr;
        long                    attr_pad[15];
    } attr;
} AudioAttributes;

typedef long                    AudioAttrMask;

typedef AudioAttrMask           ASampledAttrMask;

#define ASDataFormatMask        ( 1 << ASAFDataFormat )
#define ASBitsPerSampleMask     ( 1 << ASAFBitsPerSample )
#define ASSamplingRateMask      ( 1 << ASAFSamplingRate )
#define ASChannelsMask          ( 1 << ASAFChannels )
#define ASInterleaveMask        ( 1 << ASAFInterleave )
#define ASDurationMask          ( 1 << ASAFDuration )

typedef struct _SBucket {
    AudioAttributes             audio_attr;
    long                        max_length;
    char                      * audio_data;
} SBucket;

typedef struct _SStream {
    AudioAttributes             audio_attr;
    long                        max_block_size;
    struct sockaddr_in          tcp_sockaddr;
} SStream;

/*
 * Type for transaction ID.
 */

typedef long                    ATransID;

/*
 * Type and constants for gain specification.
 */

typedef long                    AGainDB;

#define AUnityGain              0
#define AZeroGain               0x80000000

/*
 * Structure for gain entry.
 */

typedef struct _AGainEntry {
    union {
        struct {
            AInputChType        in_ch;
            AInputSrcType       in_src;
        } i;
        struct {
            AOutputDstType      out_dst;
            AOutputChType       out_ch;
        } o;
    } u;
    AGainDB                     gain;
} AGainEntry;

/*
 * Enumerated type and structure for gain matrix.
 */

typedef enum _AGainMatrixType {
    AGMTInput,
    AGMTOutput
} AGainMatrixType;

typedef struct _AGainMatrix {
    AGainMatrixType             type;
    unsigned long               num_entries;
    AGainEntry                * gain_entries;
} AGainMatrix;

/*
 * Enumerated type for priority.
 */

typedef enum _APriority {
    APriorityUrgent,
    APriorityHigh,
    APriorityNormal,
    APriorityLow
} APriority;

/*
 * Enumerated type for transaction stop mode.
 */

typedef enum _AStopMode {
    ASMLinkedTrans,
    ASMThisTrans,
    ASMEndLoop
} AStopMode;

/*
 * Enumerated type for error code.
 */

typedef enum _AError {
    AENoError,
    AESystemCall,
    AEBadAudio,
    AEBadValue,
    AEHostNotFound,
    AENoSuchAudioNumber,
    AEBadFileFormat,
    AEBadDataFormat,
    AEFileNotFound,
    AEBadLinkID,
    AEBadGainMatrix,
    AEBadFileHdr,
    AEUnrecognizableFormat,
    AEBadAttribute,
    AEBadOffset,
    AEBadTransactionID,
    AECantDetermineFormat,
    AEOutOfMemory,
    AEOpenFailed,
    AEBadSamplingRate,
    AEBadSoundBucket,
    AEBadSoundStream,
    AETransactionBusy,
    AEllbdNotStarted,
    AERPCFailed,
    AELibraryMismatch
/* REMEMBER: add new error strings to ErrorHandlers.c */
} AError;

/*
 * Structure for metering information.
 */

typedef struct _AMonitor {
    unsigned long               sum;
    unsigned long               max;
    unsigned long               count;
} AMonitor;

/*
 * Enumerated type for transaction state.
 */

typedef enum _ATransState {
    ATSUnknownTransID,
    ATSPlaying,
    ATSRecording,
    ATSLooping,
    ATSPaused,
    ATSStopped,
    ATSLoopStopped,
    ATSPending
} ATransState;

/*
 * Structure for transaction status.
 */

typedef struct _ATransStatus {
    ATransState                 state;
    ATime                       time;
} ATransStatus;

/*
 * Enumerated type for queue check mode
 */

typedef enum _AQueueCheckMode {
    AQueuedAlready,
    AQueuedAfterReading
} AQueueCheckMode;

/*
 * Enumerated type and mask for audio events.
 */

typedef enum _AEventType {
    AETError,
    AETConnect,
    AETTransStatus,
    AETTransStarted,
    AETTransCompleted,
    AETTransStopped,
    AETTransPaused,
    AETTransResumed,
    AETTransPreempted,
    AETLoopStarted,
    AETLoopStopped,
    AETRecordMonitor,
    AETPlayMonitor,
    AETRecordData,
    AETDataNeeded,
    AETDataAvailable,
    AETGetData,
    AETDisconnect,
    AETStereoRecordMonitor,
    AETStereoPlayMonitor
} AEventType;

typedef long                    AEventMask;

#define ANoEventMask            0
#define AErrorMask                  ( 1 << AETError )
#define AConnectMask                ( 1 << AETConnect )
#define ATransStatusMask            ( 1 << AETTransStatus )
#define ATransStartedMask           ( 1 << AETTransStarted )
#define ATransCompletedMask         ( 1 << AETTransCompleted )
#define ATransStoppedMask           ( 1 << AETTransStopped )
#define ATransPausedMask            ( 1 << AETTransPaused )
#define ATransResumedMask           ( 1 << AETTransResumed )
#define ATransPreemptedMask         ( 1 << AETTransPreempted )
#define ALoopStartedMask            ( 1 << AETLoopStarted )
#define ALoopStoppedMask            ( 1 << AETLoopStopped )
#define ARecordMonitorMask          ( 1 << AETRecordMonitor )
#define APlayMonitorMask            ( 1 << AETPlayMonitor )
#define ARecordDataMask             ( 1 << AETRecordData )
#define ADataNeededMask             ( 1 << AETDataNeeded )
#define ADataAvailableMask          ( 1 << AETDataAvailable )
#define AGetDataMask                ( 1 << AETGetData )
#define ADisconnectMask             ( 1 << AETDisconnect )
#define AStereoRecordMonitorMask    ( 1 << AETStereoRecordMonitor )
#define AStereoPlayMonitorMask      ( 1 << AETStereoPlayMonitor )

/*
 * Structures for audio events.
 */

typedef struct _AAnyEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
} AAnyEvent;

typedef struct _AErrorEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    AError                      error_code;
} AErrorEvent;

typedef struct _AConnectEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    unsigned long               length;
    char                        buffer[1];      /* Variable length buffer */
} AConnectEvent;

 typedef struct _ATransStatusEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATransStatus                status;
} ATransStatusEvent;

 typedef struct _ATransStartedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       start;
} ATransStartedEvent;

typedef struct _ATransCompletedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       complete;
} ATransCompletedEvent;

typedef struct _ATransStoppedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       stop;
} ATransStoppedEvent;

typedef struct _ATransPausedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       pause;
} ATransPausedEvent;

typedef struct _ATransResumedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       resume;
} ATransResumedEvent;

typedef struct _ATransPreemptedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       preempt;
} ATransPreemptedEvent;

typedef struct _ALoopStartedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       start;
} ALoopStartedEvent;

typedef struct _ALoopStoppedEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    ATime                       stop;
} ALoopStoppedEvent;

typedef struct _ARecordMonitorEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    AMonitor                    monitor;
} ARecordMonitorEvent;

typedef struct _APlayMonitorEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    AMonitor                    monitor;
} APlayMonitorEvent;

typedef struct _ARecordDataEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    unsigned long               length;
    char                        buffer[1];      /* Variable length buffer */
} ARecordDataEvent;

typedef struct _ADataNeededEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    unsigned long               length;
} ADataNeededEvent;

typedef struct _ADataAvailableEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    unsigned long               length;
} ADataAvailableEvent;

typedef struct _AGetDataEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    long                        class_selector;
    long                        object_selector;
    long                        field_selector;
    long                        data;
} AGetDataEvent;

typedef struct _ADisconnectEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
} ADisconnectEvent;

typedef struct _AStereoRecordMonitorEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    AMonitor                    left_monitor;
    AMonitor                    right_monitor;
} AStereoRecordMonitorEvent;

typedef struct _AStereoPlayMonitorEvent {
    AEventType                  type;
    unsigned long               serial;
    ATransID                    xid;
    AMonitor                    left_monitor;
    AMonitor                    right_monitor;
} AStereoPlayMonitorEvent;

/*
 * Union of all event types.
 */

typedef union _AEvent {
    AEventType                  type;
    AAnyEvent                   any_event;
    AErrorEvent                 error_event;
    AConnectEvent               connect_event;
    ATransStatusEvent           status_event;
    ATransStartedEvent          start_event;
    ATransCompletedEvent        complete_event;
    ATransStoppedEvent          stop_event;
    ATransPausedEvent           pause_event;
    ATransResumedEvent          resume_event;
    ATransPreemptedEvent        preempt_event;
    ALoopStartedEvent           loop_start_event;
    ALoopStoppedEvent           loop_stop_event;
    ARecordMonitorEvent         record_mon_event;
    APlayMonitorEvent           play_mon_event;
    ARecordDataEvent            record_data_event;
    ADataNeededEvent            data_needed_event;
    ADataAvailableEvent         data_available_event;
    AGetDataEvent               get_data_event;
    ADisconnectEvent            disconnect_event;
    AStereoRecordMonitorEvent   stereo_record_mon_event;
    AStereoPlayMonitorEvent     stereo_play_mon_event;
    long                        event_pad[16];
} AEvent;

/*
 * Structure for sound bucket play parameters.
 */

typedef struct _SBPlayParams {
    AGainMatrix                 gain_matrix;
    AGainDB                     play_volume;
    Bool                        pause_first;
    ATime                       start_offset;
    ATime                       duration;
    long                        loop_count;
    APriority                   priority;
    ATransID                    previous_transaction;
    AEventMask                  event_mask;
} SBPlayParams;

/*
 * Structure for sound bucket record parameters.
 */

typedef struct _SBRecordParams {
    AGainMatrix                 gain_matrix;
    AGainDB                     record_gain;
    Bool                        pause_first;
    ATime                       start_offset;
    ATime                       duration;
    AEventMask                  event_mask;
} SBRecordParams;

/*
 * Structure for sound stream play parameters.
 */

typedef struct _SSPlayParams {
    AGainMatrix                 gain_matrix;
    AGainDB                     play_volume;
    APriority                   priority;
    AEventMask                  event_mask;
} SSPlayParams;

/*
 * Structure for sound stream record parameters.
 */

typedef struct _SSRecordParams {
    AGainMatrix                 gain_matrix;
    AGainDB                     record_gain;
    Bool                        pause_first;
    AEventMask                  event_mask;
} SSRecordParams;

/*
 * Enumerated type for audio connection close down mode.
 */

typedef enum _ACloseDownMode {
    ADestroyAll,
    AKeepTransactions
} ACloseDownMode;

/*
 * Enumerated type for audio file write mode.
 */

typedef enum _AWriteMode {
    AWMOverWrite,
    AWMTruncateAppend,
    AWMInsert
} AWriteMode;

/*
 * Types of system gains
 */
typedef enum _ASystemGainType {
    ASGTPlay, ASGTRecord, ASGTMonitor
} ASystemGainType;

/*
 * Structure for event queue ( private )
 */

typedef struct _ASQEvent {
    struct _ASQEvent          * next;
    AEvent                      event;
} _AQEvent;

/*
 * Structures for Extensions
 */

typedef struct _AExtData {
    long                        number;
    struct _AExtData          * next;
    long                    ( * free_private )();
    char                      * private_data;
} AExtData;

typedef struct {
    long                        extension;
    long                        major_opcode;
    long                        first_event;
    long                        first_error;
} AExtCodes;

typedef struct _AExten {
    struct _AExten            * next;
    AExtCodes                   codes;
    long                    ( * close_audio )();
    long                    ( * error )();
    long                    ( * error_string )();
    char                      * name;
} _AExtension;

/*
 * Structure for audio connection.
 */

typedef struct _Audio {
    long                        fd;
    long                        proto_major_version;
    long                        proto_minor_version;
    char                      * vendor;
    long                        vnumber;
    long                        release;
    char                      * audio_name;
    AByteOrder                  byte_order;
    ABitOrder                   sound_bit_order;
    long                        n_data_format;
    ADataFormat               * data_format_list;
    long                        n_sampling_rate;
    unsigned long             * sampling_rate_list;
    AudioAttributes           * best_audio_attr;
    long                        qlen;
    AInputSrcMask               input_sources;
    AInputChMask                input_channels;
    AOutputChMask               output_channels;
    AOutputDstMask              output_destinations;
    AGainDB                     max_input_gain;
    AGainDB                     min_input_gain;
    AGainDB                     max_output_gain;
    AGainDB                     min_output_gain;
    AGainMatrix               * simple_player_gm;
    AGainMatrix               * simple_recorder_gm;
    Bool                        gm_gain_restricted;
    char                      * adefaults;
    AGainDB                     max_monitor_gain;
    AGainDB                     min_monitor_gain;
    float                       sample_rate_hi_tolerance;
    float                       sample_rate_lo_tolerance;
    long                        alib_major_version;
    long                        alib_minor_version;
    char                     ** file_format_names;
    long                        n_file_format_names;
    char                     ** data_format_names;
    long                        n_data_format_names;

        /* The following is private to Alib */
/*    handle_t                    handle; Commented out for Praat */

    struct _Audio             * next;
    long                        lock;
    unsigned long               request;
    struct _ASQEvent          * head;
    struct _ASQEvent          * tail;
    long                        ev_buf_size;
    char                      * ev_buf;
    char                      * ev_buf_start;
    unsigned long               block_size;
    long                        stream_port;
    char                      * struct_buf;
    long                        ext_number;
    _AExtension               * ext_procs;
    AExtData                  * ext_data;
    long                        sizeof_audio;
} Audio;

typedef struct _AResampleParams {
        /* The following is private to Alib */
    double                 acc;           /* accumulator for left channel (or mono) */
    double                 acc2;          /* accumulator for right channel          */
    double                 sizfac;        /* Scale Factor: output rate/input rate   */
    double                 insfac;        /* Inverse Scale Factor: 1/sizfac         */ 
    double                 outseg;        /* number of input samples required to complete current output sample */
    double                 inseg;         /* portion of current input sample still available */
    short                  sample;        /* the current input sample for left channel (or mono) */
    short                  sample2;       /* the current input sample for right channel */
} AResampleParams;

typedef struct _AConvertParams {
        /* The following is private to Alib */
    float                    version;
    AudioAttributes          source_attributes;
    AByteOrder               source_byte_order;
    AudioAttributes          dest_attributes;
    AByteOrder               dest_byte_order;
    long                     samples_converted;
    AResampleParams        * resample_params;
} AConvertParams;

/*
 * Function prototypes
 */

extern Audio *
AOpenAudio(
#if NeedFunctionPrototypes
    char                   * /* audio_name */,
    long                   * /* status_return */
#endif
);

extern void
ACloseAudio(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    long                   * /* status_return */
#endif
);

extern void
ASetCloseDownMode(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ACloseDownMode           /* close_mode */,
    long                   * /* status_return */
#endif
);

extern long
AConnectionNumber(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern long
AProtocolVersion(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern long
AProtocolRevision(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern char *
AServerVendor(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern int
AVendorRelease(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern char *
AAudioString(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AByteOrder
ASoundByteOrder(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern ABitOrder
ASoundBitOrder(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern long
ANumDataFormats(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern ADataFormat *
ADataFormats(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern long
ANumSamplingRates(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern unsigned long *
ASamplingRates(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AudioAttributes *
ABestAudioAttributes(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern int
AQLength(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AInputSrcMask
AInputSources(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AInputChMask
AInputChannels(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AOutputChMask
AOutputChannels(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AOutputDstMask
AOutputDestinations(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainDB
AMaxInputGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainDB
AMinInputGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainDB
AMaxOutputGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainDB
AMinOutputGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainMatrix *
ASimplePlayer(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AGainMatrix *
ASimpleRecorder(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern Bool
AGMGainRestricted(
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern void
AGetSystemChannelGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ASystemGainType          /* gain_type */,
    AChType                  /* channel */,
    AGainDB                * /* gain_return */,
    long                   * /* status_return */
#endif
);

extern void
ASetSystemChannelGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ASystemGainType          /* gain_type */,
    AChType                  /* channel */,
    AGainDB                  /* gain */,
    long                   * /* status_return */
#endif
);

extern void
ASetSystemPlayGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AGainDB                  /* gain */,
    long                   * /* status_return */
#endif
);

extern void
ASetSystemRecordGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AGainDB                  /* gain */,
    long                   * /* status_return */
#endif
);

extern SBucket *
ALoadAFile(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    char                   * /* pathname */,
    AFileFormat              /* file_format */,
    AudioAttrMask            /* attr_mask */,
    AudioAttributes        * /* audio_attributes */,
    long                   * /* status_return */
#endif
);

extern void
ASaveSBucket(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    char                   * /* pathname */,
    AFileFormat              /* file_format */,
    AudioAttrMask            /* attr_mask */,
    AudioAttributes        * /* audio_attributes */,
    ATime                  * /* offset */,
    AWriteMode               /* mode */,
    long                   * /* status_return */
#endif
);

extern void
AConvertAFile(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    char                   * /* src_pathname */,
    AFileFormat              /* src_file_format */,
    char                   * /* dest_pathname */,
    AFileFormat              /* dest_file_format */,
    AudioAttrMask            /* dest_attr_mask */,
    AudioAttributes        * /* dest_attributes */,
    long                   * /* status_return */
#endif
);

extern AFileFormat
AQueryAFile(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    char                   * /* name */,
    long                   * /* status_return */
#endif
);

extern AFileFormat
AGetAFileAttributes (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    char                     *  /* pathname,        */,
    long                     *  /* offset,          */,
    long                     *  /* data_length,     */,
    AByteOrder               *  /* file_byte_order, */,
    AudioAttrMask            *  /* mask,            */,
    AudioAttributes          *  /* file_attr,       */,
    long                     *  /* status_return    */
#endif
);

extern AFileFormat
AChooseSourceAttributes (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    char                     *  /* pathname,        */,
    FILE                     *  /* audiofile,       */,
    AFileFormat                 /* user_format,     */,
    AudioAttrMask               /* user_mask,       */,
    AudioAttributes          *  /* file_attr,       */,
    long                     *  /* offset,          */,
    long                     *  /* data_length,     */,
    AByteOrder               *  /* file_byte_order, */,
    long                     *  /* status_return    */
#endif
);

extern long
ACalculateLength(
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    long                        /* src_length,       length of source buffer */,
    AudioAttributes          *  /* src_attributes,   attributes of source data */,
    AudioAttributes          *  /* dest_attributes,  attributes of destination data */,
    long                     *  /* status_return     status returned - if not null pointer */
#endif
);

extern void
AConvertBuffer (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    AConvertParams           *  /* convert_params,  */,   
    char                     *  /* src_buffer,      */,   
    long                        /* src_buffer_size, */,   
    char                     *  /* dest_buffer,     */,   
    long                        /* dest_buffer_size */,   
    long                     *  /* bytes_read,      */,   
    long                     *  /* bytes_written,   */,   
    long                     *  /* status_return    */    
#endif                                                    
);
                                                        
extern long
AGetSilenceValue(
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    ADataFormat                 /* data_format        */,
    long                      * /* significant_bytes, */,
    long                      * /* status_return      */ 
#endif                                                    
);
                                                        
extern void
AChoosePlayAttributes(
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    AudioAttributes          *  /* src_attributes,    attributes of source data */,
    AudioAttrMask               /* dest_mask  specifies those attributes already selected */,
    AudioAttributes          *  /* dest_attributes,   destination attributes */,
    AByteOrder               *  /* dest_byte_order,   returns suggested destination byte order */,
    long                     *  /* status_return */  
#endif                                                    
);

extern AConvertParams *
ASetupConversion (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    AudioAttributes          *  /* src_attributes,    attributes of source data */,
    AByteOrder               *  /* src_byte_order,    byte ordering of source data */,
    AudioAttributes          *  /* dest_attributes,   attributes of destination data */,
    AByteOrder               *  /* dest_byte_order,   byte ordering of destination data */,
    long                     *  /* status_return */  
#endif                                                    
);

extern void
AEndConversion (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           */,
    AConvertParams           *  /* convert_params,  */,   
    char                     *  /* dest_buffer,     */,   
    long                        /* dest_buffer_size */,   
    long                     *  /* bytes_written,   */,   
    long                     *  /* status_return    */    
#endif                                                    
);
                                                        
extern long
AGetDataFormats(
#if NeedFunctionPrototypes
    AFileFormat                 /* file_format     */
#endif                                                    
);

extern void
AGetFileFmtStr(
#if NeedFunctionPrototypes
    Audio                     * /* audio,        */,
    AFileFormat                 /* format,       */,
    char                      * /* buffer_return,*/,
    int                         /* buffer_length,*/,
    long                      * /* status_return */
#endif                                                    
);

extern void
AGetDataFmtStr(
#if NeedFunctionPrototypes
    Audio                     * /* audio,        */,
    ADataFormat                 /* format,       */,
    char                      * /* buffer_return,*/,
    int                         /* buffer_length,*/,
    long                      * /* status_return */
#endif                                                    
);
                                                        

extern long
AWriteAFileHeader (
#if NeedFunctionPrototypes
    Audio                    *  /* audio */,          
    char                     *  /* pathname */,      
    AFileFormat                 /* file_format */,   
    AudioAttributes          *  /* audio_attributes */,
    long                     *  /* status_return ) */  
#endif                                                    
);


extern void
AChooseAFileAttributes (
#if NeedFunctionPrototypes
    Audio                    *  /* audio,           audio structure */,
    AudioAttributes          *  /* src_attributes,  attributes of the source */,
    AFileFormat                 /* file_format,     format of target file */,
    AudioAttrMask               /* valuemask,       specifies which attributes are already selected */,
    AudioAttributes          *  /* dest_attributes, returns recommended attributes for audio file */,
    AByteOrder               *  /* dest_byte_order, return byte ordering for audio file */,
    long                     *  /* status_return )  status returned -if not null pointer */
#endif                                                    
);

extern void
AUpdateDataLength (
#if NeedFunctionPrototypes
    Audio                    *  /* audio */,          
    char                     *  /* pathname */,      
    AFileFormat                 /* file_format */,    
    long                     *  /* status_return ) */ 
#endif                                                    
);

                                    
extern ATransID
APlaySBucket(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    SBPlayParams           * /* pp */,
    long                   * /* status_return */
#endif
);

extern ATransID
ARecordAData(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    SBRecordParams         * /* rp */,
    long                   * /* status_return */
#endif
);

extern SBucket *
ACreateSBucket(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AudioAttrMask            /* attr_mask */,
    AudioAttributes        * /* audio_attributes */,
    long                   * /* status_return */
#endif
);

extern void
ADestroySBucket(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    long                   * /* status_return */
#endif
);

extern unsigned long
AGetSBucketData(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    unsigned long            /* start_offset */,
    char                   * /* buffer */,
    unsigned long            /* buf_len */,
    long                   * /* status_return */
#endif
);

extern unsigned long
APutSBucketData(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SBucket                * /* sb */,
    unsigned long            /* start_offset */,
    char                   * /* buffer */,
    unsigned long            /* length */,
    long                   * /* status_return */
#endif
);

extern ATransID
APlaySStream(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AudioAttrMask            /* attr_mask */,
    AudioAttributes        * /* audio_attributes */,
    SSPlayParams           * /* pp */,
    SStream                * /* sstream_return */,
    long                   * /* status_return */
#endif
);

extern ATransID
ARecordSStream(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AudioAttrMask            /* attr_mask */,
    AudioAttributes        * /* audio_attributes */,
    SSRecordParams         * /* rp */,
    SStream                * /* sstream_return */,
    long                   * /* status_return */
#endif
);

extern ATransID
AConnectRecordSStream(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    SStream                * /* remote_sstream */,
    SSRecordParams         * /* rp */,
    long                   * /* status_return */
#endif
);

extern void
AGetChannelGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AChType                  /* channel */,
    AGainDB                * /* gain_return */,
    long                   * /* status_return */
#endif
);

extern void
ASetChannelGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AChType                  /* channel */,
    AGainDB                  /* gain */,
    long                   * /* status_return */
#endif
);

extern void
AGetGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AGainDB                * /* gain_return */,
    long                   * /* status_return */
#endif
);

extern void
ASetGain(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AGainDB                  /* gain */,
    long                   * /* status_return */
#endif
);

extern void
AGetTransStatus(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    ATransStatus           * /* trans_status_return */,
    long                   * /* status_return */
#endif
);

extern void
APauseAudio(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    ATransStatus           * /* trans_status_return */,
    long                   * /* status_return */
#endif
);

extern void
AResumeAudio(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    ATransStatus           * /* trans_status_return */,
    long                   * /* status_return */
#endif
);

extern void
AStopAudio(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AStopMode                /* mode */,
    ATransStatus           * /* trans_status_return */,
    long                   * /* status_return */
#endif
);

extern Bool
AGrabServer(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    long                   * /* status_return */
#endif
);

extern void
AUngrabServer(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    long                   * /* status_return */
#endif
);

extern void
ASelectInput(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    ATransID                 /* xid */,
    AEventMask               /* event_mask */,
    long                   * /* status_return */
#endif
);

extern long
AEventsQueued(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AQueueCheckMode          /* mode */,
    long                   * /* status_return */
#endif
);

extern void
ANextEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEvent                 * /* event_return */,
    long                   * /* status_return */
#endif
);

extern void
APeekEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEvent                 * /* event_return */,
    long                   * /* status_return */
#endif
);

extern Bool
ACheckEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEvent                 * /* event_return */,
    long                   * /* status_return */
#endif
);

extern void
AMaskEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEventMask               /* event_mask */,
    AEvent                 * /* event_return */,
    long                   * /* status_return */
#endif
);

extern Bool
ACheckMaskEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEventMask               /* event_mask */,
    AEvent                 * /* event_return */,
    long                   * /* status_return */
#endif
);

extern void
APutBackEvent(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AEvent                 * /* event */,
    long                   * /* status_return */
#endif
);

typedef long                ( * AErrorHandler ) (
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AErrorEvent            * /* error_event */
#endif
);

extern AErrorHandler
ASetErrorHandler(
#if NeedFunctionPrototypes
    AErrorHandler            /* handler */
#endif
);

typedef long                ( * AIOErrorHandler ) (
#if NeedFunctionPrototypes
    Audio                  * /* audio */
#endif
);

extern AIOErrorHandler
ASetIOErrorHandler(
#if NeedFunctionPrototypes
    AIOErrorHandler          /* handler */
#endif
);

extern void
AGetErrorText(
#if NeedFunctionPrototypes
    Audio                  * /* audio */,
    AError                   /* error */,
    char                   * /* buffer_return */,
    int                      /* buffer_length */
#endif
);

extern void
ASetDefaultInput(
#if NeedFunctionPrototypes
    Audio                     * /* audio */,
    AInputSrcType               /* inputDev */,
    long                      * /* status_return */
#endif
);

extern void
ASetDefaultOutputs(
#if NeedFunctionPrototypes
    Audio                     * /* audio */,
    AOutputDstMask              /* outputDevMask */,
    long                      * /* status_return */
#endif
);


#ifdef  __cplusplus
}
#endif

#endif /* _ALIB_H_ */
