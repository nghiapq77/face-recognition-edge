#ifndef RETINA_FACE_H
#define RETINA_FACE_H

#include "NvInfer.h"
#include "NvOnnxParser.h"
#include "cuda_runtime_api.h"
#include "utils.h"
#include <opencv2/core.hpp>
#include <string>
#include <vector>

using namespace nvinfer1;

struct anchorBox {
    float cx;
    float cy;
    float sx;
    float sy;
};

class RetinaFace {
  public:
    RetinaFace(Logger gLogger, const std::string engineFile, int frameWidth, int frameHeight, int maxFacesPerScene);
    ~RetinaFace();
    std::vector<struct Bbox> findFace(cv::Mat &img);

  private:
    int m_frameWidth, m_frameHeight, m_maxFacesPerScene;
    static const int m_batchSize = 1;
    static const int m_INPUT_C = 3;
    // static const int m_INPUT_H = 480;
    // static const int m_INPUT_W = 640;
    static const int m_INPUT_H = 320;
    static const int m_INPUT_W = 320;
    static const int m_OUTPUT_SIZE_BASE =
        (m_INPUT_H / 8 * m_INPUT_W / 8 + m_INPUT_H / 16 * m_INPUT_W / 16 + m_INPUT_H / 32 * m_INPUT_W / 32) * 2;
    float m_scale_h;
    float m_scale_w;
    cv::Mat m_input;
    float m_output0[m_OUTPUT_SIZE_BASE * 4], m_output1[m_OUTPUT_SIZE_BASE * 2];
    std::vector<struct Bbox> m_outputBbox;
    float nms_threshold = 0.4;
    float bbox_threshold = 0.6;

    Logger m_gLogger;
    std::string m_engineFile;
    DataType m_dtype;
    ICudaEngine *m_engine;
    IExecutionContext *m_context;
    cudaStream_t stream;
    void *buffers[3];
    int inputIndex;
    int outputIndex0, outputIndex1;

    void loadEngine(Logger gLogger, const std::string engineFile);
    void preInference();
    void doInference(float *input, float *output0, float *output1);
    void preprocess(cv::Mat &img);
    void postprocessing(float *bbox, float *conf);
    void create_anchor_retinaface(std::vector<anchorBox> &anchor, int w, int h);
    static inline bool m_cmp(Bbox a, Bbox b);
    void nms(std::vector<Bbox> &input_boxes, float NMS_THRESH);
};

#endif
