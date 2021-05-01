#include "QtRendererVideo.h"
#include <assert.h>
#include <QResizeEvent>
#include "QtEvent.h"
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
QtRendererVideo::QtRendererVideo(QWidget *parent)
	: QWidget(parent, Qt::MSWindowsOwnDC)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);
	ui.setupUi(this);
	hwnd = (HWND)winId();
	CreateGLContext();
	wglMakeCurrent(dc, rc);
	assert(glewInit() == GLEW_OK);

	InitializeGL();
}

QtRendererVideo::~QtRendererVideo()
{
	wglMakeCurrent(NULL, NULL);
	if (rc)
		wglDeleteContext(rc);
	if (dc)
		ReleaseDC(hwnd, dc);
}

void QtRendererVideo::resizeEvent(QResizeEvent *event)
{
	glViewport(0, 0, event->size().width(), event->size().height());
	GLUpdate();
}

void QtRendererVideo::GLUpdate()
{
	QApplication::postEvent(this, new QtEvent(QtEvent::GL_Renderer));
}

bool QtRendererVideo::CreateGLContext()
{
	dc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	//ָ���ṹ��汾�ţ�����Ϊ1
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	//������Ȼ���
	pfd.cDepthBits = 24;
	//����ģ�建����λ��
	pfd.cStencilBits = 8;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

	int format = 0;
	format = ChoosePixelFormat(dc, &pfd);
	if (!format)
	{
		throw;
	}
	SetPixelFormat(dc, format, &pfd);
	rc = wglCreateContext(dc);
	return true;
}

void QtRendererVideo::Renderer()
{
	static float redv = 0;
	redv += 0.01;
	if (redv > 1)
		redv = 0;
	glClearColor(redv, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SwapBuffers(dc);
}

void QtRendererVideo::InitializeGL()
{
	glGenBuffers(
		1, //����VBO�ĸ���
		&VBO
	);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int size = sizeof(vertices);
	glBufferData(
		GL_ARRAY_BUFFER, //��������
		size, //�������Ĵ�С
		vertices,//��������
		GL_STATIC_DRAW
	);
	//GL_STATIC_DRAW//���ݼ�������ı�
	//GL_DYNAMIC_DRAW ���ݿ��ܻᷢ���ı�
	//GL_STREAM_DRAW ÿ�λ������ݶ��ᷢ���ı�
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	assert(!glGetError());
}

bool QtRendererVideo::event(QEvent* event)
{
	if (event->type() == QtEvent::GL_Renderer)
	{
		//������Ⱦ
		Renderer();
	}
	return QWidget::event(event);
}