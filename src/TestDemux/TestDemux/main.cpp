//���װ
#include <iostream>
#include <thread>
extern "C" {
#include "libavformat/avformat.h"
}
using namespace std;
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"avcodec.lib")
static double r2d(AVRational r)
{
	return r.den == 0 ? 0:(double)r.num / (double)r.den;
}
void XSleep(int ms)
{
	//c++ 11
	chrono::milliseconds du(ms);
	this_thread::sleep_for(du);
}

int main(int argc, char* argv[])
{
	cout << "Test Demux FFmpeg.club" << endl;
	const char* path = "test.mp4";
	//��ʼ����װ��
	av_register_all(); //���������ffmepg4.0�����Ƽ�ʹ�ã�ֱ��ע�ͼ��ɣ�Ҳ�ɹر�sdl���
	//��ʼ������⣨���Դ�rtsp rtmp httpЭ�����ý����Ƶ��
	avformat_network_init();
	
	//��������
	AVDictionary* opts = NULL;
	//����rtsp����tcpЭ���
	av_dict_set(&opts, "rstp_transport", "tcp", 0);
	//������ʱʱ��
	av_dict_set(&opts, "max_delay", "500", 0);

	//���װ������
	AVFormatContext* ic = NULL;
	int re = avformat_open_input(
		&ic,
		path,
		0,  // 0��ʾ�Զ�ѡ������
		&opts //�������ã�����rtsp����ʱʱ��
	);
	if (re != 0) //�ɹ�����ֵΪ0
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "open " << path << " failed!:" << buf << endl;
		getchar();
		return -1;
	}
	cout << "open " << path << " success!" << endl;

	//��ȡ����Ϣ
	re = avformat_find_stream_info(ic, 0);

	//��ʱ�� ����
	int total = ic->duration / (AV_TIME_BASE/1000);
	cout << "totalMs = " << total << endl;

	//��ӡ��Ƶ����ϸ��Ϣ
	av_dump_format(ic, 0, path, 0); //�ڶ�������û�ã�����������Ҳû��Ӱ�죬���ĸ��������Ƿ��������

	//����Ƶ��������ȡʱ��������Ƶ
	int videoStream = 0;
	int audioStream = 1;

	//���ٱ������ں�����ȡ��

	//��ȡ����Ƶ����Ϣ ��������
	for (int i = 0; i < ic->nb_streams; i++)  //nb_streams��ʾ����Ƶ���ĸ���
	{
		AVStream* as = ic->streams[i];
		cout << "codec_id = " << as->codecpar->codec_id << endl;
		cout << "format = " << as->codecpar->format << endl; //����8��ʾFLTP��float 32λ��ƽ��洢��ʽ����ͨ��˳��洢��32λ������ز���
		//��Ƶ AVMEDIA_TYPE_AUDIO
		if (as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream = i;
			cout << i << "��Ƶ��Ϣ" << endl;
			cout << "sample_rate = " << as->codecpar->sample_rate << endl; //������
			//AVSampleFormat;  ������ʽ
			cout << "channels = " << as->codecpar->channels << endl;  //ͨ����
			//һ֡����  ��ͨ��������
			cout << "frame_size = " << as->codecpar->frame_size << endl;
			//1024*2��ͨ������*2��16λ2�ֽڣ�=4096��һ֡���ݴ�С��   fps = sample_rate/frame_size

		}
		//��Ƶ AVMEDIA_TYPE_VIDEO
		else if (as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = i;
			cout << i << "��Ƶ��Ϣ" << endl;
			cout << "width = " << as->codecpar->width << endl;
			cout << "height = " << as->codecpar->height << endl;
			//֡�� fps  ����ת��  ��Ƶ��һ����fps��������Ƶ����һһ��Ӧ��
			cout << "video fps = " << r2d(as->avg_frame_rate) << endl;
		}
	}

	//��ȡ��Ƶ����������ȡ��
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, NULL,0);
	//ic->streams[videoStream];

	//malloc AVPacket����ʼ��
	AVPacket *pkt = av_packet_alloc();
	for (;;) //ʹ������ѭ����ȡ��Ƶ֡����
	{
		int re = av_read_frame(ic, pkt);
		if (re != 0)
		{
			//ѭ������ ÿ�ε���β��ת��msλ��
			cout << "----------------------end----------------------" << endl;
			int ms = 3000;//����λ�� ����ʱ�������������ת��
			//int ms = 120000;
			long long pos = (double)ms / (double)1000 * r2d(ic->streams[pkt->stream_index]->time_base);
			av_seek_frame(ic, videoStream, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME); //�����磩�����ҹؼ�֡
			continue;
		}
		cout << "pkt->size = " << pkt->size << endl;
		//��ʾ��ʱ��
		cout << "pkt->pts = " << pkt->pts << endl;
		
		//ת��Ϊ���룬������ͬ��  ��Ƶ��Ƶ��time_base��һ��
		cout << "pkt->pts ms = " << pkt->pts * (r2d(ic->streams[pkt->stream_index]->time_base) * 1000) << endl;



		//����ʱ��  ����б�֡��������ǰһ֡�ͺ�һ֡��������Ϻ�����ʾ������ʾʱ��Ϊ2�������ʱ��Ϊ3
		cout << "pkt->dts = " << pkt->dts << endl;
		if (pkt->stream_index == videoStream)
		{
			cout << "ͼ��" << endl;
		}
		if (pkt->stream_index == audioStream)
		{
			cout << "��Ƶ" << endl;
		}

		//�ͷţ����ü���-1  Ϊ0�ͷſռ�
		av_packet_unref(pkt);

		XSleep(200);
	}
	av_packet_free(&pkt);

	if (ic)
	{
		//�ͷŷ�װ�����ģ����Ұ�ic��0
		avformat_close_input(&ic);
	}

	getchar();
	return 0;
}