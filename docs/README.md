# **Film Grain Synthesis API & Usage**
## **Introduction**
This document describes usage of Film Grain Synthesis based on SMPTE-RDD5 [frequency-filtering method]. Package contains Library source and header files for film grain synthesis along with sample application files. 

## **FGS - API’s and Data Structures**
This section contains details on various API calls supported by Film Grain Synthesis and Data structures used for the API calls.

## **API functions**	

|**Function name**|**Inputs**|**Outputs**|**Notes**|
| - | :-: | :- | :-: |
|fgs\_init|<p>GrainCharacteristicApi</p><p></p>|<p>FGS handle pointer</p><p></p>|Allocates handle for FGS internal GrainSynthesisStruct and generates data base of grain samples.|
|fgs\_deinit|<p>FGS handle pointer</p><p></p>|None|Free the memory allocated for internal structure GrainSynthesisStruct.|
|<p>fgs\_simulation\_blending</p><p> </p>|<p>GrainCharacteristicApi,</p><p>FGS handle pointer</p><p></p>|<p>errorCode</p><p></p>|Frame level call to perform grain simulation and blending. Validates FGC SEI for SMPTE-RDD5 and input YUV parameters|
|<p>fgs\_validate\_input\_params</p><p></p>|<p>GrainCharacteristicApi</p><p></p>|<p>errorCode</p><p></p>|Facilitates validation of FGC SEI for SMPTE-RDD5 and input YUV parameters |

Please refer to the flow of the calls in the figure below.

<p align="center">
<img src="https://github.com/ittiam-systems/libfgs/blob/main/docs/FGS_Flow_Diagram.png" />
</p>

<p align="center"><b>Film Grain Synthesizer- flow of API functions</b></p>

## **Data Structures**
### **GrainCharacteristicApi**
~~~
typedef struct
{
void *decPel;
uint32_t width;
uint32_t height;
uint8_t chromaFormat;
uint8_t bitDepth;
uint32_t poc; 
uint32_t idrPicId; 
uint8_t enableDeblocking;
uint32_t errorCode;
FilmGrainCharacteristicsStruct fgcParameters; 
void *psFgsHandle;
} GrainCharacteristicApi;
~~~

|Variable|Description|
| - | - |
|decPel|decoder output samples|
|width|width of the frame|
|height|height of the frame|
|chromaFormat|0 : monochrome; 1: 420; 2: 422;|
|bitDepth|8,10 bits|
|poc|decoded frame picture order count|
|idrPicId|Derived from slice header; 0: non-IDR pics|
|enableDeblocking|Enable de-blocking at 8x8 boundaries|
|errorCode|Return error from Film grain synthesizer|
|fgcParameters|Film grain characteristics|
|psFgsHandle|Film grain Synthesizer internal handle|
### **FilmGrainCharacteristicsStruct** 
~~~
typedef struct
{
uint8_t filmGrainCharacteristicsCancelFlag; 
uint8_t filmGrainModelId; 
uint8_t separateColourDescriptionPresentFlag; 
uint8_t filmGrainBitDepthLumaMinus8;
uint8_t filmGrainBitDepthChromaMinus8;
uint8_t filmGrainFullRangeFlag;
uint8_t filmGrainColourPrimaries;
uint8_t filmGrainTransferCharacteristics;
uint8_t filmGrainMatrixCoefficients;
uint8_t blendingModeId; 
uint8_t log2ScaleFactor; 
uint8_t compModelPresentFlag[MAX_NUM_COMP];
uint8_t numIntensityIntervalsMinus1[MAX_NUM_COMP];
uint8_t numModelValuesMinus1[MAX_NUM_COMP];
uint8_t intensityIntervalLowerBound[MAX_NUM_COMP][MAX_NUM_INTENSITIES];
uint8_t intensityIntervalUpperBound[MAX_NUM_COMP][MAX_NUM_INTENSITIES];
uint32_t compModelValue[MAX_NUM_COMP][MAX_NUM_INTENSITIES][MAX_NUM_MODEL_VALUES]; 
uint32_t filmGrainCharacteristicsRepetitionPeriod; 
} FilmGrainCharacteristicsStruct;
~~~
**Note:** Intensity intervals are limited by standard 0-255. For bit depths greater than 8, the block average at 8x8 level is down shifted by a factor by (bit\_depth -8) to compare intensity interval boundaries.

|Variable|Description|
| - | - |
|filmGrainCharacteristicsCancelFlag |0 : to perform film grain synthesis|
|filmGrainModelId|0 : frequency filtering model|
|separateColourDescriptionPresentFlag|0 : Decoded samples and grain to be in same color space|
|filmGrainBitDepthLumaMinus8|Not Applicable|
|filmGrainBitDepthChromaMinus8|Not Applicable|
|filmGrainFullRangeFlag|Not Applicable|
|filmGrainColourPrimaries|Not Applicable|
|filmGrainTransferCharacteristics|Not Applicable|
|filmGrainMatrixCoefficients|Not Applicable|
|blendingModeId|0 : Additive blending|
|log2ScaleFactor|2-7 : scale factor used in film grain simulation|
|compModelPresentFlag|Control for component model for each component|
|numIntensityIntervalsMinus1|Number of intensity intervals in each component|
|numModelValuesMinus1|0-2 : Number of model values in each component |
|intensityIntervalLowerBound|Lower bound of intensity interval : 0-255|
|intensityIntervalUpperBound|Upper bound of intensity interval: 0-255^^|
|compModelValue|Component model values for each intensity interval|
|filmGrainCharacteristicsRepetitionPeriod|0:  Persistence of the film grain characteristics|
 # **Glossary**

|Term|Explanation|
| :-: | :-: |
|FGS|Film Grain Synthesizer|
|FGC|Film Grain Characteristics|
|SEI|supplemental enhancement information|
|HBD|High Bit Depth|

 # **Error Codes**

|**ENUM**|**ERROR CODE**|**COMMENTS**|
| :- | :- | :- |
|FGS\_SUCCESS|0x00|No error|
|FGS\_INVALID\_WIDTH|0x11|Invalid input width|
|FGS\_INVALID\_HEIGHT|0x12|Invalid input height|
|FGS\_INVALID\_CHROMA\_FORMA|0x13|Invalid Chroma format idc|
|FGS\_INVALID\_BIT\_DEPTH|0x14|Invalid bit depth|
|FGS\_INVALID\_FGC\_CANCEL\_FLAG|0x21|Invalid Film grain characteristic cancel flag|
|FGS\_INVALID\_GRAIN\_MODEL\_ID|0x22|Invalid film grain model id|
|FGS\_INVALID\_SEP\_COL\_DES\_FLAG|0x23|Invalid separate color description present flag|
|FGS\_INVALID\_BLEND\_MODE|0x24|Invalid blending mode|
|FGS\_INVALID\_LOG2\_SCALE\_FACTOR|0x25|Invalid log\_2\_scale\_factor value|
|FGS\_INVALID\_COMP\_MODEL\_PRESENT\_FLAG|0x26|Invalid component model present lag|
|FGS\_INVALID\_NUM\_MODEL\_VALUES|0x27|Invalid number of model values|
|FGS\_INVALID\_INTENSITY\_BOUNDARY\_VALUES|0x28|Invalid bound values, overlapping boundaries|
|FGS\_INVALID\_STANDARD\_DEVIATION|0x29|Invalid standard deviation|
|FGS\_INVALID\_CUT\_OFF\_FREQUENCIES|0x2A|Invalid cut off frequencies|
|FGS\_INVALID\_NUM\_CUT\_OFF\_FREQ\_PAIRS|0x2B|Invalid number of cut off frequency pairs|
|FGS\_INVALID\_FGC\_REPETETION\_PERIOD|0x2C|Invalid film grain characteristics repetition period|
|FGS\_FAIL|0xFF|Failure error code|

 # **Macros**

|Macro|Value|
| :- | :- |
|MAX\_NUM\_COMP|3|
|MAX\_NUM\_INTENSITIES|256|
|MAX\_NUM\_MODEL\_VALUES|6|



