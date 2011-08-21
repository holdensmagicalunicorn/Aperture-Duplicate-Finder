/*
 * Aperture.h
 */

#import <AppKit/AppKit.h>
#import <ScriptingBridge/ScriptingBridge.h>


@class ApertureApplication, ApertureContainer, ApertureAlbum, ApertureCustomTag, ApertureEXIFTag, ApertureExportSetting, ApertureFileNamingPolicy, ApertureFolder, ApertureFolderNamingPolicy, ApertureImageVersion, ApertureIPTCTag, ApertureKeyword, ApertureLibrary, ApertureOtherTag, ApertureProject, ApertureSubfolder, ApertureTrashFolder, ApertureImageAdjustmentPreset, ApertureWindow, ApertureRichText, ApertureCharacter, ApertureParagraph, ApertureWord, ApertureAttributeRun, ApertureAttachment;

enum ApertureImportOptions {
	ApertureImportOptionsCopying = 'rkcp' /* Import by copying files (default option). */,
	ApertureImportOptionsReferencing = 'rkrf' /* Import by referencing files. */,
	ApertureImportOptionsMoving = 'rkmv' /* Import by moving files. */
};
typedef enum ApertureImportOptions ApertureImportOptions;

enum ApertureMetadataTypes {
	ApertureMetadataTypesSidecar = 'rksc' /* Create IPTC4XMP sidecar files when exporting masters. */,
	ApertureMetadataTypesEmbedded = 'rkeb' /* Embed IPTC metadata in exported masters. */
};
typedef enum ApertureMetadataTypes ApertureMetadataTypes;

// Color labels
enum ApertureColorLabels {
	ApertureColorLabelsNoColorLabel = 'rkNL' /* No color label */,
	ApertureColorLabelsRed = 'rkRD' /* Red color label */,
	ApertureColorLabelsOrange = 'rkOR' /* Orange color label */,
	ApertureColorLabelsYellow = 'rkYL' /* Yellow color label */,
	ApertureColorLabelsGreen = 'rkGR' /* Green color label */,
	ApertureColorLabelsBlue = 'rkBL' /* Blue color label */,
	ApertureColorLabelsPurple = 'rkPU' /* Purple color label */,
	ApertureColorLabelsGray = 'rkGY' /* Gray color label */
};
typedef enum ApertureColorLabels ApertureColorLabels;

enum ApertureSaveOptions {
	ApertureSaveOptionsYes = 'yes ' /* Save the file. */,
	ApertureSaveOptionsNo = 'no  ' /* Do not save the file. */,
	ApertureSaveOptionsAsk = 'ask ' /* Ask the user whether or not to save the file. */
};
typedef enum ApertureSaveOptions ApertureSaveOptions;

enum AperturePrintingErrorHandling {
	AperturePrintingErrorHandlingStandard = 'lwst' /* Standard PostScript error handling */,
	AperturePrintingErrorHandlingDetailed = 'lwdt' /* print a detailed report of PostScript errors */
};
typedef enum AperturePrintingErrorHandling AperturePrintingErrorHandling;



/*
 * Aperture Suite
 */

// The application's top-level scripting object.
@interface ApertureApplication : SBApplication

- (SBElementArray *) windows;
- (SBElementArray *) projects;
- (SBElementArray *) folders;
- (SBElementArray *) subfolders;
- (SBElementArray *) albums;
- (SBElementArray *) imageVersions;
- (SBElementArray *) libraries;
- (SBElementArray *) fileNamingPolicies;
- (SBElementArray *) folderNamingPolicies;
- (SBElementArray *) exportSettings;
- (SBElementArray *) imageAdjustmentPresets;

@property (copy, readonly) NSString *name;  // The name of the application.
@property (readonly) BOOL frontmost;  // Is this the active application?
@property (copy, readonly) NSString *version;  // The version number of the application.
@property (copy, readonly) NSArray *selection;  // List of image version(s) currently selected in the browser.
@property (copy, readonly) NSArray *tasks;  // List of active tasks.
@property BOOL fullscreen;  // Is the application in fullscreen mode?
@property (copy, readonly) ApertureTrashFolder *trash;  // application trash container

- (id) import:(NSArray *)x by:(ApertureImportOptions)by into:(id)into;  // Import image versions, masters, or projects into the specified Aperture container.
- (NSArray *) export:(NSArray *)x namingFoldersWith:(ApertureFolderNamingPolicy *)namingFoldersWith using:(id)using_ metadata:(ApertureMetadataTypes)metadata to:(id)to namingFilesWith:(ApertureFileNamingPolicy *)namingFilesWith consolidatingImages:(BOOL)consolidatingImages;  // Export image versions, masters, or projects to the specified location as files or Aperture libraries.
- (BOOL) adjustImageDate:(NSDate *)x ofImages:(NSArray *)ofImages mastersIncluded:(BOOL)mastersIncluded;  // Adjust the value of the EXIF Image Date tag of an image.  The command has the ability to adjust the value of the tag of an image version or both the image version and the master file.
- (void) reveal:(id)x;  // Reveal image versions or containers.
- (void) restore:(id)x;  // Restore a container(s) from the Aperture Trash.
- (void) refreshMetadata:(id)x;  // Update the EXIF metadata from the master file.
- (void) print:(id)x withProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) quitSaving:(ApertureSaveOptions)saving;  // Quit the application.
- (BOOL) exists:(id)x;  // Verify that an object exists.

@end

// Every container.
@interface ApertureContainer : SBObject

- (SBElementArray *) containers;
- (SBElementArray *) imageVersions;

@property (copy, readonly) ApertureContainer *parent;  // parent container
@property (copy) NSString *name;  // container name
- (NSString *) id;  // unique identifier

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) restore;  // Restore a container(s) from the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an album, smart album, light table, web gallery, web journal, or book within an Aperture library.
@interface ApertureAlbum : ApertureContainer

- (SBElementArray *) imageVersions;


@end

// This class represents a custom tag of an image version.
@interface ApertureCustomTag : SBObject

@property (copy) NSString *value;  // This is the value of the custom tag.
@property (copy, readonly) NSString *name;  // This is the name of the custom tag.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an EXIF tag of an image version.
@interface ApertureEXIFTag : SBObject

@property (copy, readonly) id value;  // This is the value of the EXIF tag.
@property (copy, readonly) NSString *name;  // This is the name of the EXIF tag.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// A collection of settings that describe an export operation.
@interface ApertureExportSetting : SBObject

@property (copy, readonly) NSString *name;  // The name of the export setting.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// A collection of settings that describe the file naming format of an exported image.
@interface ApertureFileNamingPolicy : SBObject

@property (copy, readonly) NSString *name;  // The name of the file naming policy.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents a folder within an Aperture library.
@interface ApertureFolder : ApertureContainer

- (SBElementArray *) albums;
- (SBElementArray *) folders;
- (SBElementArray *) projects;


@end

// A collection of settings that describe the folder naming format of an exported image.
@interface ApertureFolderNamingPolicy : SBObject

@property (copy, readonly) NSString *name;  // The name of the folder naming policy.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an Aperture image version.
@interface ApertureImageVersion : SBObject

- (SBElementArray *) keywords;
- (SBElementArray *) customTags;
- (SBElementArray *) IPTCTags;
- (SBElementArray *) EXIFTags;
- (SBElementArray *) otherTags;

@property (readonly) NSInteger width;  // The width of the image version.
@property (readonly) NSInteger height;  // The height of the image version.
@property NSInteger mainRating;  // The main rating for the image version.
@property (readonly) BOOL picked;  // Is this image version the pick of its stack?
- (NSString *) id;  // The unique identifier of the image version.
@property (readonly) BOOL selected;  // Is this image version part of the selection?
@property (copy) NSString *name;  // The name of the image version.
@property (readonly) BOOL online;  // Is the image version's master file online.
@property (readonly) BOOL referenced;  // Is the image version's master file referenced.
@property (copy, readonly) ApertureContainer *parent;  // The parent container.
@property double latitude;  // The GPS latitude in range of -90.0 to 90.0.
@property double longitude;  // The GPS longitude in range of -180.0 to 180.0. Measurement is taken from the prime meridian, so 'west' longitudes are negative numbers.
@property BOOL flagged;  // Is this image version flagged?
@property ApertureColorLabels colorLabel;  // The color label of the image version.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) refreshMetadata;  // Update the EXIF metadata from the master file.
- (void) applyImageAdjustmentPreset:(ApertureImageAdjustmentPreset *)preset replacingExistingAdjustments:(BOOL)replacingExistingAdjustments;  // Apply an image adjustment preset to an image version.
- (void) resetAllImageAdjustments;  // Reset an image version's adjustments.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an IPTC tag of an image version.
@interface ApertureIPTCTag : SBObject

@property (copy) NSString *value;  // Value of the IPTC tag.
@property (copy, readonly) NSString *name;  // Name of the IPTC tag.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an IPTC keyword tag of an image version.
@interface ApertureKeyword : SBObject

@property (copy, readonly) NSString *name;  // Name of the keyword.
@property (copy, readonly) NSString *parents;  // A tab-separated list of the parent keywords.
- (NSString *) id;  // The unique identifier of the keyword.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This represents an Aperture library.
@interface ApertureLibrary : ApertureContainer

- (SBElementArray *) projects;
- (SBElementArray *) folders;
- (SBElementArray *) albums;


@end

// This class represents other metadata of an image version.
@interface ApertureOtherTag : SBObject

@property (copy, readonly) id value;  // Value of the other tag.
@property (copy, readonly) NSString *name;  // Name of the other tag.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// This class represents an Aperture project.
@interface ApertureProject : ApertureContainer

- (SBElementArray *) imageVersions;
- (SBElementArray *) albums;
- (SBElementArray *) subfolders;


@end

// This class represents a subfolder within an Aperture project or subfolder.
@interface ApertureSubfolder : ApertureContainer

- (SBElementArray *) albums;
- (SBElementArray *) subfolders;


@end

// trash container
@interface ApertureTrashFolder : ApertureContainer


@end

// This class represents an image adjustment preset.
@interface ApertureImageAdjustmentPreset : SBObject

@property (copy, readonly) NSString *name;  // Image Adjustment Preset name.
- (NSString *) id;  // Image Adjustment Preset identifier.
@property (copy, readonly) NSString *group;  // Preset group name.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end



/*
 * Standard Suite
 */

// A window.
@interface ApertureWindow : SBObject

@property (copy, readonly) NSString *name;  // The title of the window.
- (NSInteger) id;  // The unique identifier of the window.
@property NSInteger index;  // The index of the window, ordered front to back.
@property NSRect bounds;  // The bounding rectangle of the window.
@property (readonly) BOOL closeable;  // Does the window have a close button?
@property (readonly) BOOL miniaturizable;  // Does the window have a minimize button?
@property BOOL miniaturized;  // Is the window minimized right now?
@property (readonly) BOOL resizable;  // Can the window be resized?
@property BOOL visible;  // Is the window visible right now?
@property (readonly) BOOL zoomable;  // Does the window have a zoom button?
@property BOOL zoomed;  // Is the window zoomed right now?

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end



/*
 * Text Suite
 */

// Rich (styled) text.
@interface ApertureRichText : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the text's first character.
@property (copy) NSString *font;  // The name of the font of the text's first character.
@property NSInteger size;  // The size in points of the text's first character.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// One of some text's characters.
@interface ApertureCharacter : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // Its color.
@property (copy) NSString *font;  // The name of its font.
@property NSInteger size;  // Its size, in points.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// One of some text's paragraphs.
@interface ApertureParagraph : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the paragraph's first character.
@property (copy) NSString *font;  // The name of the font of the paragraph's first character.
@property NSInteger size;  // The size in points of the paragraph's first character.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// One of some text's words.
@interface ApertureWord : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // The color of the word's first character.
@property (copy) NSString *font;  // The name of the font of the word's first character.
@property NSInteger size;  // The size in points of the word's first character.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// A chunk of text that all has the same attributes.
@interface ApertureAttributeRun : SBObject

- (SBElementArray *) characters;
- (SBElementArray *) paragraphs;
- (SBElementArray *) words;
- (SBElementArray *) attributeRuns;
- (SBElementArray *) attachments;

@property (copy) NSColor *color;  // Its color.
@property (copy) NSString *font;  // The name of its font.
@property NSInteger size;  // Its size, in points.

- (void) duplicateTo:(SBObject *)to;  // Duplicate an image version from an album or project to an album or project.
- (void) moveTo:(SBObject *)to;  // Move an image version from an album or project to an album or project.
- (void) empty;  // Empty the Aperture Trash.
- (void) closeSaving:(ApertureSaveOptions)saving savingIn:(NSURL *)savingIn;  // Close a document.
- (void) saveIn:(NSURL *)in_;  // Save a document.
- (void) printWithProperties:(NSDictionary *)withProperties printDialog:(BOOL)printDialog;  // Print a document.
- (void) delete;  // Delete an object.

@end

// A file embedded in text. This is just for use when embedding a file using the make command.
@interface ApertureAttachment : ApertureRichText

@property (copy) NSString *fileName;  // The path to the embedded file.


@end

