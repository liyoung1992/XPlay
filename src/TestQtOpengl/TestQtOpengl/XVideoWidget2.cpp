/*******************************************************************************
**                                                                            **
**                     Jiedi(China nanjing)Ltd.                               **
**	               �������Ĳܿ����˴��������Ϊѧϰ�ο�                       **
*******************************************************************************/

/*****************************FILE INFOMATION***********************************
**
** Project       : FFmpeg
** Description   : FFMPEG��Ŀ����ʾ��
** Contact       : xiacaojun@qq.com
**        ����   : http://blog.csdn.net/jiedichina
**		��Ƶ�γ�
**�����ƿ���	http://study.163.com/u/xiacaojun
**��Ѷ����		https://jiedi.ke.qq.com/
**csdnѧԺ		http://edu.csdn.net/lecturer/lecturer_detail?lecturer_id=961
**51ctoѧԺ	    http://edu.51cto.com/lecturer/index/user_id-12016059.html
**�������µ�ffmpeg�汾 http://www.ffmpeg.club
**
**   ffmpeg+qt������ ѧԱȺ ��462249121 ����Ⱥ���ش���ͽ���
**   ΢�Ź��ں�  : jiedi2007
**		ͷ����	 : �Ĳܿ�
**
*******************************************************************************/
//������������������ ѧԱ��Ⱥ462249121���ش���ͽ���
 
#include "XVideoWidget.h"
#include <QDebug>

//�Զ���˫����
#define GET_STR(x) #x

//����shader
const char *vString = GET_STR(
	attribute vec4 vertexIn;
	attribute vec2 textureIn;
	varying vec2 textureOut;
	void main(void)
	{
		gl_Position = vertexIn;
		textureOut = textureIn;
	}
);


//ƬԪshader
const char *tString = GET_STR(
	varying vec2 textureOut;
	uniform sampler2D tex_y;
	uniform sampler2D tex_u;
	uniform sampler2D tex_v;
	void main(void)
	{
		vec3 yuv;
		vec3 rgb;
		yuv.x = texture2D(tex_y, textureOut).r;
		yuv.y = texture2D(tex_u, textureOut).r - 0.5;
		yuv.z = texture2D(tex_v, textureOut).r - 0.5;
		rgb = mat3(1.0, 1.0, 1.0,
			0.0, -0.39465, 2.03211,
			1.13983, -0.58060, 0.0) * yuv;
		gl_FragColor = vec4(rgb, 1.0);
	}

);



//׼��yuv����
// ffmpeg -i v1080.mp4 -t 10 -s 240x128 -pix_fmt yuv420p  out240x128.yuv
XVideoWidget::XVideoWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

XVideoWidget::~XVideoWidget()
{
}

//��ʼ��opengl
void XVideoWidget::initializeGL()
{
	qDebug() << "initializeGL";
	
	//��ʼ��opengl ��QOpenGLFunctions�̳У����� 
	initializeOpenGLFunctions();

	//program����shader�������ƬԪ���ű�
	//ƬԪ�����أ�
	qDebug()<<program.addShaderFromSourceCode(QGLShader::Fragment, tString);
	//����shader
	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);

}

//ˢ����ʾ
void XVideoWidget::paintGL()
{
	qDebug() << "paintGL";
}


// ���ڳߴ�仯
void XVideoWidget::resizeGL(int width, int height)
{
	qDebug() << "resizeGL "<<width<<":"<<height;
}