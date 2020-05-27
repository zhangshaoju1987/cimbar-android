#pragma once

#include "concurrent_fountain_decoder_sink.h"
#include "encoder/Decoder.h"
#include "extractor/Extractor.h"

#include "concurrent/thread_pool.h"
#include <opencv2/opencv.hpp>

class MultiThreadedDecoder
{
public:
	MultiThreadedDecoder(std::string data_path);

	inline static clock_t bytes = 0;
	inline static clock_t decoded = 0;
	inline static clock_t ticks = 0;

	bool add(const cv::Mat& img);
	cv::Mat pop();

	void stop();

protected:
	Extractor _ext;
	Decoder _dec;
	turbo::thread_pool _pool;
	concurrent_fountain_decoder_sink<599> _writer;
};

inline MultiThreadedDecoder::MultiThreadedDecoder(std::string data_path)
	: _ext()
	, _dec(0)
	, _pool(std::thread::hardware_concurrency(), 1)
    , _writer(data_path)
{
	_pool.start();
}

inline bool MultiThreadedDecoder::add(const cv::Mat& img)
{
	return _pool.try_execute( [&, img] () {
		clock_t begin = clock();
		bytes += _dec.decode(img, _writer);
		++decoded;
		ticks += clock() - begin;
	} );
}

inline void MultiThreadedDecoder::stop()
{
	_pool.stop();
}
