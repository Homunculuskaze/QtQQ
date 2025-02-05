#include "ReceiveFile.h"
#include <QFileDialog>
#include <QMessageBox>

//全局变量，文件名称
extern QString gfileName;
//全局变量，数据
extern QString gfileData;


ReceiveFile::ReceiveFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);

	iniTitleBar();
	setTitleBarTitle("", ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(100, 400);

	//如果用户没有点确定，也不点取消，而是点右上角的X，也是拒绝接收效果
	connect(m_titleBar, &TitleBar::signalButtonCloseClicked, this, &ReceiveFile::refuseFile);
}

ReceiveFile::~ReceiveFile()
{}

void ReceiveFile::setMsg(QString & msgLabel)
{
	ui.label->setText(msgLabel);
}


//确定
void ReceiveFile::on_okBtn_clicked()
{
	this->close();

	//获取想要保存文件的路径
	QString fileDirPath = QFileDialog::getExistingDirectory(
		nullptr,			//父类给个空值，因为说不定已经在上一句close了
		QStringLiteral("文件保存路径"),
		"/");

	//获取文件整体路径，带上文件名一起
	//文件名称
	//之前在解析数据时候，就解析到了文件名称了，
	//用一个全局变量拿过来
	QString filePath = fileDirPath + "/" + gfileName;

	QFile file(filePath);			//整出来一个文件对象


	//判断文件是否写入成功
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::information(nullptr,
			QStringLiteral("提示"), QStringLiteral("文件接收失败!"));
	}
	else
	{
		//接收成功，开始写入数据
		file.write(gfileData.toUtf8());
		file.close();

		QMessageBox::information(nullptr, QStringLiteral("提示"),
			QStringLiteral("文件接收成功!"));
	}
}

//取消
void ReceiveFile::on_cancelBtn_clicked()
{
	emit refuseFile();				//取消
	this->close();					//关闭
}
