#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <mpg123.h>
#include <kiss_fft.h>
#include <cstring> // for memcpy

// 假设的辅助函数：将解码后的PCM数据转换为浮点数并进行FFT计算主要频率
float findDominantFrequency(const std::vector<short>& pcmSamples, int sampleRate) {
    // 1. 准备FFT输入数据 (应用汉宁窗并转换为float)
    size_t n = pcmSamples.size();
    std::vector<float> windowedSamples(n);
    kiss_fft_cpx* fft_input = new kiss_fft_cpx[n];
    kiss_fft_cpx* fft_output = new kiss_fft_cpx[n];

    for (size_t i = 0; i < n; ++i) {
        // 应用汉宁窗减少频谱泄漏
        float window = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (n - 1)));
        windowedSamples[i] = window * static_cast<float>(pcmSamples[i]);
        fft_input[i].r = windowedSamples[i];
        fft_input[i].i = 0.0f;
    }

    // 2. 执行FFT
    kiss_fft_cfg cfg = kiss_fft_alloc(n, 0, nullptr, nullptr);
    kiss_fft(cfg, fft_input, fft_output);
    kiss_fft_free(cfg);
    delete[] fft_input;

    // 3. 计算幅度谱并寻找最大幅度对应的频率
    float maxMagnitude = -1.0f;
    int maxIndex = 0;
    for (size_t i = 0; i < n / 2; ++i) { // 只关心前一半（实信号频谱对称）
        float magnitude = std::sqrt(fft_output[i].r * fft_output[i].r + fft_output[i].i * fft_output[i].i);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxIndex = i;
        }
    }
    delete[] fft_output;

    // 4. 计算主导频率 (maxIndex * sampleRate / n)
    float dominantFrequency = (maxIndex * sampleRate) / static_cast<float>(n);
    return dominantFrequency;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input.mp3>" << std::endl;
        return 1;
    }

    const char* filename = argv[1];
    mpg123_handle* mh = nullptr;
    int err = MPG123_OK;

    // 初始化libmpg123
    err = mpg123_init();
    if (err != MPG123_OK) {
        std::cerr << "Failed to initialize mpg123: " << mpg123_plain_strerror(err) << std::endl;
        return 1;
    }

    mh = mpg123_new(nullptr, &err);
    if (mh == nullptr) {
        std::cerr << "Failed to create mpg123 handle: " << mpg123_plain_strerror(err) << std::endl;
        mpg123_exit();
        return 1;
    }

    // 打开MP3文件
    if (mpg123_open(mh, filename) != MPG123_OK) {
        std::cerr << "Failed to open file: " << filename << " - " << mpg123_strerror(mh) << std::endl;
        mpg123_delete(mh);
        mpg123_exit();
        return 1;
    }

    // 获取音频格式信息
    long sampleRate;
    int channels, encoding;
    if (mpg123_getformat(mh, &sampleRate, &channels, &encoding) != MPG123_OK) {
        std::cerr << "Failed to get format: " << mpg123_strerror(mh) << std::endl;
        mpg123_close(mh);
        mpg123_delete(mh);
        mpg123_exit();
        return 1;
    }

    // 确保输出格式为Signed 16-bit（便于处理）
    mpg123_format_none(mh);
    mpg123_format(mh, sampleRate, channels, MPG123_ENC_SIGNED_16);

    std::cout << "Decoding MP3: " << filename << " (Sample Rate: " << sampleRate << " Hz, Channels: " << channels << ")" << std::endl;

    size_t buffer_size = mpg123_outblock(mh); // 建议的解码缓冲区大小
    std::vector<unsigned char> decodeBuffer(buffer_size);
    std::vector<short> pcmSamples; // 存储所有解码后的PCM样本（交错）

    // 解码循环
    size_t done;
    int totalFrames = 0;
    do {
        err = mpg123_read(mh, decodeBuffer.data(), decodeBuffer.size(), &done);
        if (err == MPG123_OK) {
            // 将解码后的数据（16位有符号PCM）添加到pcmSamples
            size_t samplesRead = done / sizeof(short);
            pcmSamples.insert(pcmSamples.end(),
                reinterpret_cast<short*>(decodeBuffer.data()),
                reinterpret_cast<short*>(decodeBuffer.data()) + samplesRead);
        }
        else if (err != MPG123_DONE) {
            std::cerr << "Decoding error: " << mpg123_strerror(mh) << std::endl;
            break;
        }
    } while (err != MPG123_DONE);

    std::cout << "Decoded " << pcmSamples.size() / channels << " samples." << std::endl;

    // 如果音频是双声道，转换为单声道（取平均值）
    std::vector<short> monoSamples;
    if (channels == 2) {
        for (size_t i = 0; i < pcmSamples.size(); i += 2) {
            monoSamples.push_back((pcmSamples[i] + pcmSamples[i + 1]) / 2);
        }
    }
    else {
        monoSamples = pcmSamples; // 已经是单声道
    }

    // 分帧处理并计算每帧的主导频率
    const size_t frameSize = 1024;  // 每帧样本数（例如1024）
    const size_t hopSize = 512;      // 帧移（重叠50%）
    std::vector<float> dominantFrequencies;

    for (size_t start = 0; start + frameSize < monoSamples.size(); start += hopSize) {
        std::vector<short> frame(monoSamples.begin() + start, monoSamples.begin() + start + frameSize);
        float freq = findDominantFrequency(frame, sampleRate);
        dominantFrequencies.push_back(freq);
        std::cout << "Frame " << (start / hopSize) << ": Dominant Frequency ≈ " << freq << " Hz" << std::endl;
    }

    // 将频率数据输出到文件（供51单片机使用）
    std::ofstream outFile("extracted_pitch_data.txt");
    for (float freq : dominantFrequencies) {
        outFile << freq << std::endl;
    }
    outFile.close();
    std::cout << "Pitch data written to extracted_pitch_data.txt" << std::endl;

    // 清理资源
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    return 0;
}