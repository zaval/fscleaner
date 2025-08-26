//
// Created by Dmytrii Zavalnyi on 13.08.2025..
//

#include "mediafileinfo.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/dict.h>
}

MediaFileInfo::MediaFileInfo() {


}

MediaFileInfo::FileInfo MediaFileInfo::getMediaInfo(const QString &filename) {
    FileInfo info;
    info.filename = filename;
    AVFormatContext* format_ctx = nullptr;

    if (avformat_open_input(&format_ctx, filename.toStdString().c_str(), nullptr, nullptr) != 0) {
        return info;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(format_ctx, nullptr) < 0) {
        avformat_close_input(&format_ctx);
        return info;
    }

    int audio_stream_index = -1;
    int video_stream_index = -1;
    for (int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_index = i;
        }
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
        }
    }

    if (audio_stream_index == -1 && video_stream_index == -1) {
        avformat_close_input(&format_ctx);
        return info;
    }

    if (audio_stream_index != -1) {
        info.audio.is_valid = true;
        const AVStream* audio_stream = format_ctx->streams[audio_stream_index];
        const auto *acodec_params = audio_stream->codecpar;

        info.audio.sample_rate = acodec_params->sample_rate;
        info.audio.channels = acodec_params->ch_layout.nb_channels;
        info.audio.bit_rate = acodec_params->bit_rate;
        info.audio.format_name = format_ctx->iformat->name;

        AVRational time_base = audio_stream->time_base;
        int64_t duration_in_stream_units = audio_stream->duration;
        double duration_seconds = duration_in_stream_units * av_q2d(time_base);
        info.audio.duration = duration_seconds;


        // Get codec information
        if (const AVCodec* codec = avcodec_find_decoder(acodec_params->codec_id)) {
            info.audio.codec_name = codec->name;
        }

        const AVDictionaryEntry* tag = nullptr;
        while ((tag = av_dict_get(format_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            info.audio.metadata[tag->key] = tag->value;
        }

        // Also check stream metadata
        while ((tag = av_dict_get(audio_stream->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            info.audio.metadata[tag->key] = tag->value;
        }
    }


    if (video_stream_index != -1) {
        info.video.is_valid = true;
        const AVStream* video_stream = format_ctx->streams[video_stream_index];
        const auto *vcodec_params = video_stream->codecpar;
        info.video.bit_rate = vcodec_params->bit_rate;
        info.video.width = vcodec_params->width;
        info.video.height = vcodec_params->height;
        info.video.format_name = format_ctx->iformat->name;

        const AVRational time_base = video_stream->time_base;
        const int64_t duration_in_stream_units = video_stream->duration;
        const double duration_seconds = duration_in_stream_units * av_q2d(time_base);
        info.video.duration = duration_seconds;

        if (const AVCodec* codec = avcodec_find_decoder(vcodec_params->codec_id)) {
            info.video.codec_name = codec->name;
        }

        const AVDictionaryEntry* tag = nullptr;
        while ((tag = av_dict_get(format_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            info.video.metadata[tag->key] = tag->value;
        }

        while ((tag = av_dict_get(video_stream->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
            info.audio.metadata[tag->key] = tag->value;
        }
    }

    avformat_close_input(&format_ctx);
    info.is_valid = true;

    return info;

}

QString MediaFileInfo::getDurationString(double duration) {
    int hours = static_cast<int>(duration / 3600.0);
    int minutes = static_cast<int>(fmod(duration, 3600.0) / 60.0);
    int seconds = static_cast<int>(fmod(duration, 60.0));
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}
