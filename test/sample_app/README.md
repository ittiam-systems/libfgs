# **Film Grain Synthesis Sample Application**

## **Configuration Parameters and Limitations**
This section describes the various parameters passed to the application using the configuration file. 
## **Sequence Level - File I/O Parameters**

|**Parameter**|**Description**|**Range / Format**|**Notes**|
| - | :-: | :- | :-: |
|input\_yuv\_file\_name|Name of input YUV file with full path or relative path |YUV 4:2:0 / 4:2:2 with 8, 10 bit support.|Application exits if input file does not exist.|
|output\_yuv\_file\_name|Name of output YUV file with full path or relative path|Format will be same as input YUV.|Application exits if output file cannot be created.|
|num\_frames|Number of frames to process|NA|NA|
|Width|Width of the decoded frame|128 -7680|In multiples of  4 |
|height|Height of the decoded frame|128 - 4320|In multiples of  4|
|bit\_depth|Bit depth of the decoded samples|8,10|NA|
|chromaFormat|Chroma format idc of the decoded samples|0-3|NA|
|enable\_deblocking|Controls de-blocking at 8x8 boundaries|0-1|NA|

  **Table 1.1 Sequence Level - File I/O Parameters**

Sequence level File I/O parameters are placed in between identifies “INPUT\_PARAMS\_START” and “INPUT\_PARAMS\_END” at start of the config file. Sample config file is captured below.
~~~
#Input Params
INPUT_PARAMS_START   
input_yuv_file_name           =   /home/yuvs/input/stripes4_color_8bit_420.yuv
output_yuv_file_name          =   /home/yuvs/output/stripes4_color_8bit_420_grain.yuv
num_frames                    =   500
width                         =   1920
height                        =   1080
bit_depth                     =   8
chroma_format                 =   1
INPUT_PARAMS_END
~~~

## **Frame level - Film Grain Parameters**

|**Parameter**|**Description**|**Range** |**Notes**|
| :-: | :-: | :-: | :-: |
|start\_frame\_num|Frame number from FGC parameters are applicable|NA|To be less than or equal to end\_frame\_num|
|end\_frame\_num|Frame number till FGC parameters are applicable|NA|To be more than start\_frame\_num|
|film\_grain\_characteristics\_cancel\_flag|Control for presence of FGC SEI params.|0-1|Controls grain synthesis at frame level.|
|film\_grain\_model\_id|Film grain model for simulation|0-1|Valid value is 0.|
|separate\_colour\_description\_present\_flag|Separate color format for decoded samples and grain|0-1|Valid value is 0.|
|blending\_mode\_id|Blending mode|0-1|Valid value is 0.|
|log2\_scale\_factor|Scale factor used in film grain simulation|0-15|Valid value is 2-7|
|comp\_model\_present\_flag|Control for component model for each component|0-1|NA|
|num\_intensity\_intervals\_minus1|Number of intensity intervals in each component |0-255|Non over lapping intervals.|
|num\_model\_values\_minus1|Number of model values in each component|0-5|Valid values 0-2|
|intensity\_interval\_lower\_bound|Lower bound of intensity interval|0-255^^|Non over lapping intervals(( for all bit depths)|
|intensity\_interval\_upper\_bound|Upper bound of intensity interval|0-255^^|Non over lapping intervals( for all bit depths)|
|<p>comp\_model\_value</p><p></p>|Component model values for each intensity interval|\*\*|<p>##</p><p></p>|

**Table 1.2 Frame level Film Grain Parameters**

**Notes:** 
\*\* - allowed values - comp\_model\_value[0]  - [0,( (2^bitdepth)-1)]
comp\_model\_value[1]  - [0,16]
comp\_model\_value[2]  - [0,16] 

\## - allowed values - comp\_model\_value[1]  - [2,14]
comp\_model\_value[2]  - [2,14] 

^^ - Intensity intervals are limited by standard 0-255. For bit depths greater than 8, the block average at 8x8 level is down shifted by a factor by ( bit\_depth -8) to compare intensity interval boundaries.

Frame level film grain characteristics parameters are placed in between identifies “FGC\_PARAMS\_PROFILE\_START” and “FGC\_PARAMS\_PROFILE\_END”.
Multiple such identifiers can be used to pass different combinations of Film grain characteristics. Sample config file is captured below.
 ### **Scenario:**
In the following example, frames 50 to 110 use FGC with 3 component models as follows

- Luma component 
  - One intensity interval [0 – 255],
  - Three component model value [80,6,14] (sigma, h,v)
- Cb component 
  - One intensity interval [160 – 255],
  - Two component model value [40,6] (sigma, h) (v=h inferred)
- Cr component 
  - One intensity interval [50 – 255],
  - One component model value [64] (sigma)  (v=h=8 is inferred)
~~~
#FGC Params Profile 01
FGC_PARAMS_PROFILE_START
start_frame_num                            =  50
end_frame_num                              =  110
film_grain characteristics_cancel_flag     =  0
log2_scale_factor                          =  3
comp_model_present_flag                    =  1,1,1
num_intensity_intervals_minusl             =  0,0,0
num_model_values_minusl                    =  2,1,0
intensity_interval_lower_bound             =  0,160,50
intensity_interval_upper_bound             =  255,255,255
comp_model_value                           =  80,6,14,40,6,64
FGC_PARAMS_PROFILE_END
~~~

|**Parameter**|**Value**|
| ------ | ------ |
|filmGrainCharacteristicsCancelFlag|0|
|log2ScaleFactor|3|
|compModelPresentFlag[c]|{1, 1, 1}|
|numintensityintervalsMinus1[c]|{0, 0, 0}|
|numModelValuesMinus1[c]|{2, 1, 0}|
|intensityIntervallowerBound[c][i]|{{0},{160},{50}}|
|intensityIntervalUpperBound[c][i]|{{255),{255},{255}}|
|compModelValue[c][i][j]|{{{80, 6, 14}},{{40, 6}},{{64}}}|

