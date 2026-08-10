#include <UIKit/UIKit.h>
#import <AudioToolbox/AudioServices.h>

extern "C" void AudioServicesPlaySystemSoundWithVibration(SystemSoundID inSystemSoundID, id unknown, NSDictionary *options);

typedef enum Action : NSInteger {
	home,
	lock,
	switcher,
	reachability,
	siri,
	screenshot,
	cc,
	nc,
	nothing,
	lastApp,
	rotationLock,
	rotatePortraitAndLock
} Action;

#define kIdentifier @"com.dgh0st.dvirtualhome"
#define kSettingsPath @"/var/mobile/Library/Preferences/com.dgh0st.dvirtualhome.plist"
#define kSettingsChangedNotification (CFStringRef)@"com.dgh0st.dvirtualhome/settingschanged"

static BOOL isEnabled = YES;
static Action singleTapAction = home;
static Action doubleTapAction = switcher;
static Action longHoldAction = reachability;
static Action tapAndHoldAction = siri;
static BOOL isVibrationEnabled = YES;
static CGFloat vibrationIntensity = 0.75;
static NSInteger vibrationDuration = 30; 

@interface SBReachabilityManager : NSObject
+ (id)sharedInstance;
- (void)toggleReachability;
@end

@interface SBScreenshotManager
- (void)saveScreenshotsWithCompletion:(id)arg1;
@end

@interface SBUIController : NSObject
+ (id)sharedInstance;
- (BOOL)handleHomeButtonDoublePressDown;
@end

@interface SpringBoard : UIApplication
@property (nonatomic, retain) NSString *lastApplicationIdentifier;
@property (nonatomic, retain) NSString *currentApplicationIdentifier;
- (void)_simulateHomeButtonPressWithCompletion:(id)arg1;
- (void)_simulateHomeButtonPress;
- (void)_simulateLockButtonPress;
- (SBScreenshotManager *)screenshotManager;
- (void)takeScreenshot;
- (id)_accessibilityTopDisplay;
- (id)_accessibilityFrontMostApplication;
- (UIInterfaceOrientation)_frontMostAppOrientation;
@end

@interface SBControlCenterController // ≤ iOS 18
+ (id)sharedInstance;
- (BOOL)isVisible;
- (void)dismissAnimated:(BOOL)arg1;
- (void)presentAnimated:(BOOL)arg1;
@end

@interface SBControlCenterCoordinator // iOS 26
+ (id)sharedInstance;
- (BOOL)isVisible;
- (void)dismissAnimated:(BOOL)arg1;
- (void)presentAnimated:(BOOL)arg1;
@end

@interface SBNotificationCenterController
+ (id)sharedInstance;
- (BOOL)isVisible;
- (void)dismissAnimated:(BOOL)arg1;
- (void)presentAnimated:(BOOL)arg1;
@end

@interface SBCoverSheetSlidingViewController : UIViewController
- (void)_dismissCoverSheetAnimated:(BOOL)arg1 withCompletion:(id)arg2;
- (void)_presentCoverSheetAnimated:(BOOL)arg1 withCompletion:(id)arg2;
- (void)_presentCoverSheetAnimated:(BOOL)arg1 forUserGesture:(BOOL)arg2 withCompletion:(id)arg3;
@end

@interface SBCoverSheetPresentationManager
@property (retain, nonatomic) SBCoverSheetSlidingViewController *coverSheetSlidingViewController;
@property (retain, nonatomic) SBCoverSheetSlidingViewController *secureAppSlidingViewController;
+ (id)sharedInstance;
- (BOOL)hasBeenDismissedSinceKeybagLock;
- (BOOL)isVisible;
- (BOOL)isInSecureApp;
@end

@interface UIGestureRecognizer (DVirtualHome)
- (id)initWithTarget:(id)arg1 action:(SEL)arg2;
- (id)allowedPressTypes;
- (void)setAllowedPressTypes:(id)arg1;
- (void)requireGestureRecognizerToFail:(id)arg1;
- (void)setDelegate:(id)arg1;
@end

@interface UIHBClickGestureRecognizer : UIGestureRecognizer
- (void)setClickCount:(NSInteger)arg1;
- (void)_resetGestureRecognizer;
@end

@interface SBHBDoubleTapUpGestureRecognizer : UIHBClickGestureRecognizer
@end

@interface SBSystemGestureManager
+ (id)mainDisplayManager;
- (id)display;
@end

@interface SBHomeHardwareButtonGestureRecognizerConfiguration
@property (retain, nonatomic) UIHBClickGestureRecognizer *singleTapGestureRecognizer;
@property (retain, nonatomic) UILongPressGestureRecognizer *longTapGestureRecognizer;
@property (retain, nonatomic) UILongPressGestureRecognizer *tapAndHoldTapGestureRecognizer;
@property (retain, nonatomic) UILongPressGestureRecognizer *vibrationGestureRecognizer;
@property (retain, nonatomic) UIHBClickGestureRecognizer *initialButtonDownGestureRecognizer;
- (id)doubleTapUpGestureRecognizer;
- (SBSystemGestureManager *)systemGestureManager;
@end

@interface SBHomeHardwareButton : NSObject <UIGestureRecognizerDelegate>
- (id)gestureRecognizerConfiguration;
- (void)createSingleTapGestureRecognizerWithConfiguration:(id)arg1;
- (void)createLongTapGestureRecognizerWithConfiguration:(id)arg1;
- (void)createTapAndHoldGestureRecognizerWithConfiguration:(id)arg1;
- (void)createVibrationGestureRecognizerWithConfiguration:(id)arg1;
- (void)performAction:(Action)action;
@end

@interface FBSystemGestureManager : NSObject
+ (id)sharedInstance;
- (void)addGestureRecognizer:(id)arg1 toDisplay:(id)arg2;
- (void)addGestureRecognizer:(id)arg1 toDisplayWithIdentity:(id)arg2;
@end

@interface SBMainDisplaySystemGestureManager  : NSObject
+ (id)sharedInstance;
- (void)addGestureRecognizer:(id)arg1 ttoDisplayWithIdentity:(id)arg2;
@end

@interface SBApplicationProcessState : NSObject // iOS 11+
@property (getter=isRunning, nonatomic, readonly) BOOL running;
@end

@interface SBApplication : NSObject
@property (nonatomic,readonly) SBApplicationProcessState *processState; // iOS 11+
- (NSString *)bundleIdentifier;
- (BOOL)isRunning; // iOS 10
@end

@interface SBApplicationController : NSObject
+ (id)sharedInstance;
- (id)applicationWithBundleIdentifier:(id)arg1;
@end

@interface SBMainWorkspace : NSObject
+ (id)sharedInstance;
- (id)createRequestForApplicationActivation:(id)arg1 options:(NSUInteger)arg2;
- (BOOL)executeTransitionRequest:(id)arg1;
@end

@interface SBWorkspaceApplication : NSObject
+ (id)entityForApplication:(id)arg1;
@end

@interface SBDeviceApplicationSceneEntity : NSObject
- (id)initWithApplicationForMainDisplay:(id)arg1;
@end

@interface SBWorkspaceTransitionRequest : NSObject
@end

@interface SBOrientationLockManager : NSObject
+ (id)sharedInstance;
- (BOOL)isUserLocked;
- (void)lock:(UIInterfaceOrientation)arg1;
- (void)unlock;
@end

@interface UIDevice (DVirtualHome)
- (void)setOrientation:(NSInteger)arg1;
@end

@interface SBBacklightController
+ (id)sharedInstance;
- (BOOL)screenIsOn;
@end
