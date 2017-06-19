#include "qt/window/QtLicense.h"

#include <QApplication>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>

#include "LicenseChecker.h"
#include "qt/utility/utilityQt.h"
#include "utility/ResourcePaths.h"

QtLicense::QtLicense(QWidget *parent)
	: QtWindow(false, parent)
{
}

QSize QtLicense::sizeHint() const
{
	return QSize(780, 520);
}

void QtLicense::clear()
{
	if (m_licenseText)
	{
		m_licenseText->setText("");
	}

	if (m_errorLabel)
	{
		m_errorLabel->setText(" ");
	}
}

void QtLicense::load()
{
	clear();

	std::string licenseString = LicenseChecker::getInstance()->getCurrentLicenseString();

	if (licenseString.size() && m_licenseText)
	{
		m_licenseText->setText(licenseString.c_str());
	}

	updateCloseButton("Cancel");
}

void QtLicense::setErrorMessage(const QString& errorMessage)
{
	if (m_errorLabel)
	{
		m_errorLabel->setText(errorMessage);
	}
}

void QtLicense::populateWindow(QWidget* widget)
{
	QVBoxLayout* subLayout = new QVBoxLayout();
	subLayout->setContentsMargins(270, 0, 0, 0);

	QLabel* licenseName = new QLabel();
	licenseName->setText(QString::fromLatin1("Enter Licence"));
	licenseName->setObjectName("title");
	subLayout->addWidget(licenseName);

	subLayout->addSpacing(10);

	QLabel* licenseIntro = new QLabel();
	licenseIntro->setText(QString::fromLatin1("Please enter a licence key to activate Sourcetrail:"));
	licenseIntro->setObjectName("licenseIntro");
	subLayout->addWidget(licenseIntro);

	m_licenseText = new QTextEdit();
	m_licenseText->setObjectName("licenseField");
	m_licenseText->setAcceptRichText(false);
	m_licenseText->setPlaceholderText(
		"-----BEGIN LICENSE-----\n"
		"Product: Sourcetrail\n"
		"Licensed to:\n"
		"License type:\n"
		"Valid up to version:\n"
		"-\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n"
		"xxxxxxxxxxxxxx\n"
		"-----END LICENSE-----\n"
	);
	subLayout->addWidget(m_licenseText);

	m_errorLabel = new QLabel(this);
	m_errorLabel->setObjectName("licenseError");
	m_errorLabel->setText(" ");
	subLayout->addWidget(m_errorLabel);

	subLayout->addSpacing(10);

	QLabel* linkLabel = new QLabel(this);
	linkLabel->setObjectName("linkLabel");
	linkLabel->setText(
		"Don't have a license key yet? "
		"Please <a href=\"http://sourcetrail.com/buy-license\" style=\"color: #007AC2;\">purchase a license</a>, "
		"or get a temporary <a href=\"http://sourcetrail.com/test-license\" style=\"color: #007AC2;\">test license</a>.");
	linkLabel->setOpenExternalLinks(true);
	linkLabel->setWordWrap(true);
	linkLabel->setGeometry(275, 300, 300, 50);
	subLayout->addWidget(linkLabel);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addLayout(subLayout);
	layout->addSpacing(20);

	widget->setLayout(layout);
}

void QtLicense::windowReady()
{
	m_content->setStyleSheet(m_content->styleSheet() + utility::getStyleSheet(ResourcePaths::getGuiPath().concat(FilePath("license/license.css"))).c_str());

	addLogo();

	updateNextButton("Activate");
	setPreviousVisible(false);

	m_title->hide();
}

void QtLicense::handleNext()
{
	std::string licenseString = m_licenseText->toPlainText().toStdString();

	LicenseChecker* checker = LicenseChecker::getInstance().get();
	LicenseChecker::LicenseState state = checker->checkLicenseString(licenseString);

	std::string errorString;

	switch (state)
	{
		case LicenseChecker::LICENSE_EMPTY:
			errorString = "No licence key was entered.";
			break;
		case LicenseChecker::LICENSE_MOVED:
		case LicenseChecker::LICENSE_MALFORMED:
			errorString = "The entered license key is malformed.";
			break;
		case LicenseChecker::LICENSE_INVALID:
			errorString = "The entered license key is invalid.";
			break;
		case LicenseChecker::LICENSE_EXPIRED:
			errorString = "The entered license key is expired.";
			break;
		case LicenseChecker::LICENSE_VALID:
		{
			checker->saveCurrentLicenseString(licenseString);
			m_errorLabel->setText(" ");

			emit finished();
			return;
		}
	}

	m_errorLabel->setText(errorString.c_str());
}
