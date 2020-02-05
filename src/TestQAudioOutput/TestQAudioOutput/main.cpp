//��Ƶ����
#include <QtCore/QCoreApplication>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QThread>
//׼����pcm����
//ffmpeg -i test.mp4 -f s16le out.pcm  short int����16λ leС��ģʽ
int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	QAudioFormat fmt;
	fmt.setSampleRate(48000);  //����ͨ����������ʵ�ֿ����ŵ�Ч��
	fmt.setSampleSize(16);
	fmt.setChannelCount(2);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);
	QAudioOutput* out = new QAudioOutput(fmt);
	QIODevice* io = out->start(); //��ʼ����

	int size = out->periodSize();
	char* buf = new char[size];

	FILE* fp = fopen("out.pcm", "rb");
	while (!feof(fp))
	{
		if (out->bytesFree() < size)
		{
			QThread::msleep(1);
			continue;
		}
		int len = fread(buf, 1, size, fp);
		if (len <= 0)	break;
		io->write(buf, len);
	}
	fclose(fp);
	delete buf;
	buf = 0;


	
	return a.exec();
}
