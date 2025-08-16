#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

bool isBackingUp = false;

class DumbPopup final : public Popup<>, GJAccountBackupDelegate {
protected:
	bool setup() override {
		m_noElasticity = true;
		this->setTitle("FOO BAR TEST POPUP REMOVE THIS LATER"_spr);
		CCMenuItemSpriteExtra* backupButton = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Backup", 1.f),
			this, menu_selector(DumbPopup::onBackup)
		);
		CCMenuItemSpriteExtra* closeButton = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Close Game", 1.f),
			this, menu_selector(DumbPopup::onCloseGame)
		);
		m_buttonMenu->addChild(backupButton);
		backupButton->setPosition(128, 100);
		m_buttonMenu->addChild(closeButton);
		closeButton->setPosition(128, 50);
		return true;
	}
public:
	void onBackup(CCObject* sender) {
		if (isBackingUp) return;
		GJAccountManager::get()->getAccountBackupURL();
		isBackingUp = true;
	}
	void onCloseGame(CCObject* sender) {
		if (MenuLayer* menuLayer = CCScene::get()->getChildByType<MenuLayer>(0)) menuLayer->endGame();
	}
	virtual void backupAccountFinished() {
		isBackingUp = false;
	}
    virtual void backupAccountFailed(BackupAccountError, int) {
		isBackingUp = false;
	}
	static DumbPopup* create() {
		auto ret = new DumbPopup();
		const CCSize ws = CCDirector::get()->getWinSize() / 2.f;
		if (ret && ret->initAnchored(256, 160)) {
			ret->autorelease();
			return ret;
		}
		delete ret;
		return nullptr;
	}
};

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		auto bottomMenu = getChildByID("bottom-menu");
		if (!bottomMenu) return true;
		bottomMenu->addChild(
			CCMenuItemSpriteExtra::create(
				CircleButtonSprite::createWithSpriteFrameName(
					"pixelitem_139_001.png", 1.f, CircleBaseColor::Green,
					CircleBaseSize::MediumAlt
				), this, menu_selector(MyMenuLayer::onDumbPopup)
			)
		);
		bottomMenu->updateLayout();
		return true;
	}
	void onDumbPopup(CCObject*) {
		if (isBackingUp) return;
		if (DumbPopup* dp = DumbPopup::create(); dp) dp->show();
	}
};