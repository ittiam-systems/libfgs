typedef struct FilmGrainCharacteristicsStruct_t FilmGrainCharacteristicsStruct;

void create_fgs(uint32_t width,uint32_t height, uint8_t bits);
void FGS_process(void* Y,void* U,void* V,unsigned int strideY,unsigned int strideUV);
void setFGSparams(FilmGrainCharacteristicsStruct *sei);
void FGS_delete(void);
void *get_outBufY(void);
void *get_outBufU(void);
void *get_outBufV(void);
void *get_inpBufY(void);
void *get_inpBufU(void);
void *get_inpBufV(void);