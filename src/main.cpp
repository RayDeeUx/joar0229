#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

bool isBackingUp = false;

class DumbPopup final : public Popup<>, GJAccountBackupDelegate {
protected:
	void onClose(CCObject* sender) override {
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		Popup::onClose(sender);
	}
	bool setup() override {
		m_noElasticity = true;
		this->setTitle("Control Panel"_spr);
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
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		auto gjam = GJAccountManager::get();
		gjam->m_backupDelegate = this;
		bool result = gjam->getAccountBackupURL();
		isBackingUp = true;
		Notification::create(fmt::format("Backing up now... (Status: {})", result))->show();
	}
	void onCloseGame(CCObject* sender) {
		if (isBackingUp) return Notification::create("Slow down there! I'm still backing up.")->show();
		if (MenuLayer* menuLayer = CCScene::get()->getChildByType<MenuLayer>(0)) menuLayer->endGame();
	}
	virtual void backupAccountFinished() {
		log::info("backupAccountFinished called");
		GJAccountManager::get()->m_backupDelegate = nullptr;
		Notification::create("Backup finished. It succeeded.")->show();
		isBackingUp = false;
	}
    virtual void backupAccountFailed(BackupAccountError p0, int p1) {
		log::info("backupAccountFailed called");
		GJAccountManager::get()->m_backupDelegate = nullptr;
		Notification::create(fmt::format("Backup finished. It failed. (p0: {}, p1: {})", static_cast<int>(p0), p1))->show();
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
					"pixelitem_139_001.png", .75f, CircleBaseColor::Green,
					CircleBaseSize::MediumAlt
				), this, menu_selector(MyMenuLayer::onDumbPopup)
			)
		);
		bottomMenu->updateLayout();
		return true;
	}
	void onDumbPopup(CCObject*) {
		if (DumbPopup* dp = DumbPopup::create(); dp) dp->show();
	}
};