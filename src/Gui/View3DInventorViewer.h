/***************************************************************************
 *   Copyright (c) 2004 Juergen Riegel <juergen.riegel@web.de>             *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#ifndef GUI_VIEW3DINVENTORVIEWER_H
#define GUI_VIEW3DINVENTORVIEWER_H

#include <list>
#include <map>
#include <set>
#include <vector>

#include <Base/Type.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/SbRotation.h>
#include <Gui/Quarter/SoQTQuarterAdaptor.h>
#include <QCursor>
#include <QImage>

#include <Gui/Selection.h>

class SoTranslation;
class SoTransform;
class SoText2;
namespace Quarter = SIM::Coin3D::Quarter;

class SoSeparator;
class SoShapeHints;
class SoMaterial;
class SoRotationXYZ;
class SbSphereSheetProjector;
class SoEventCallback;
class SbBox2s;
class SoVectorizeAction;
class QGLFramebufferObject;
class QImage;
class SoGroup;

namespace Gui {

class ViewProvider;
class SoFCBackgroundGradient;
class NavigationStyle;
class SoFCUnifiedSelection;
class Document;
class SoFCUnifiedSelection;
class GLGraphicsItem;
class SoShapeScale;
class ViewerEventFilter;

/** GUI view into a 3D scene provided by View3DInventor
 *
 */
class GuiExport View3DInventorViewer : public Quarter::SoQTQuarterAdaptor, public Gui::SelectionSingleton::ObserverType
{
    typedef Quarter::SoQTQuarterAdaptor inherited;
    
public:
    /// Pick modes for picking points in the scene
    enum SelectionMode {
        Lasso       = 0,  /**< Select objects using a lasso. */
        Rectangle   = 1,  /**< Select objects using a rectangle. */
        Rubberband  = 2,  /**< Select objects using a rubberband. */
        BoxZoom     = 3,  /**< Perform a box zoom. */
        Clip        = 4,  /**< Clip objects using a lasso. */
    };
    /** @name Modus handling of the viewer
      * Here the you can switch on/off several features
      * and modies of the Viewer
      */
    //@{
    enum ViewerMod {
        ShowCoord=1,       /**< Enables the Coordinate system in the corner. */
        ShowFPS  =2,       /**< Enables the Frams per Second counter. */
        SimpleBackground=4,/**< switch to a simple background. */
        DisallowRotation=8,/**< switch of the rotation. */
        DisallowPanning=16,/**< switch of the panning. */
        DisallowZooming=32,/**< switch of the zooming. */
	useVBO=64,/**< switch of the OpenGL VBO usage. */
    };
    //@}
    
    /** @name Anti-Aliasing modes of the rendered 3D scene
      * Specifies Anti-Aliasing (AA) method
      * - Smoothing enables OpenGL line and vertex smoothing (basicly depreciated)
      * - MSAA is hardware multi sampling (with 2, 4 or 8 passes), a quite commom and efficient AA technique
      */
    //@{
    enum AntiAliasing {
        None,
        Smoothing,
        MSAA2x,
        MSAA4x,
        MSAA8x
    };
    //@}

    /** @name Render mode
      */
    //@{
    enum RenderType {
        Native,
        Framebuffer,
        Image
    };
    //@}

    View3DInventorViewer (QWidget *parent, const QGLWidget* sharewidget = 0);
    View3DInventorViewer (const QGLFormat& format, QWidget *parent, const QGLWidget* sharewidget = 0);
    virtual ~View3DInventorViewer();
    
    void init();

    /// Observer message from the Selection
    virtual void OnChange(Gui::SelectionSingleton::SubjectType &rCaller,
                          Gui::SelectionSingleton::MessageType Reason);

    SoDirectionalLight* getBacklight(void) const;
    void setBacklight(SbBool on);
    SbBool isBacklight(void) const;
    void setSceneGraph (SoNode *root);
    SbBool searchNode(SoNode*) const;

    void setAnimationEnabled(const SbBool enable);
    SbBool isAnimationEnabled(void) const;

    void setPopupMenuEnabled(const SbBool on);
    SbBool isPopupMenuEnabled(void) const;

    void startAnimating(const SbVec3f& axis, float velocity);
    void stopAnimating(void);
    SbBool isAnimating(void) const;

    void setFeedbackVisibility(const SbBool enable);
    SbBool isFeedbackVisible(void) const;

    void setFeedbackSize(const int size);
    int getFeedbackSize(void) const;

    void setRenderType(const RenderType type);
    RenderType getRenderType() const;
    void renderToFramebuffer(QGLFramebufferObject*);

    virtual void setViewing(SbBool enable);
    virtual void setCursorEnabled(SbBool enable);

    void addGraphicsItem(GLGraphicsItem*);
    void removeGraphicsItem(GLGraphicsItem*);
    std::list<GLGraphicsItem*> getGraphicsItems() const;
    std::list<GLGraphicsItem*> getGraphicsItemsOfType(const Base::Type&) const;
    void clearGraphicsItems();

    /** @name Handling of view providers */
    //@{
    SbBool hasViewProvider(ViewProvider*) const;
    /// adds an ViewProvider to the view, e.g. from a feature
    void addViewProvider(ViewProvider*);
    /// remove a ViewProvider
    void removeViewProvider(ViewProvider*);
    /// get view provider by path
    ViewProvider* getViewProviderByPath(SoPath*) const;
    ViewProvider* getViewProviderByPathFromTail(SoPath*) const;
    /// get all view providers of given type
    std::vector<ViewProvider*> getViewProvidersOfType(const Base::Type& typeId) const;
    /// set the ViewProvider in special edit mode
    SbBool setEditingViewProvider(Gui::ViewProvider* p, int ModNum=0);
    /// return whether a view provider is edited
    SbBool isEditingViewProvider() const;
    /// reset from edit mode
    void resetEditingViewProvider();
    /// display override mode
    void setOverrideMode(const std::string &mode);
    void updateOverrideMode(const std::string &mode);
    std::string getOverrideMode() {return overrideMode;}
    //@}

    /** @name Making pictures */
    //@{
    /**
     * Creates an image with width \a w and height \a h of the current scene graph
     * and exports the rendered scenegraph to an image.
     */
    void savePicture(int w, int h, const QColor&, QImage&) const;
    void saveGraphic(int pagesize, const QColor&, SoVectorizeAction* va) const;
    //@}
    /**
     * Writes the current scenegraph to an Inventor file, either in ascii or binary. 
     */
    bool dumpToFile(SoNode* node, const char* filename, bool binary) const;

    /** @name Selection methods */
    //@{
    void startSelection(SelectionMode = Lasso);
    void stopSelection();
    bool isSelecting() const;
    std::vector<SbVec2f> getGLPolygon(SbBool* clip_inner=0) const;
    std::vector<SbVec2f> getGLPolygon(const std::vector<SbVec2s>&) const;
    const std::vector<SbVec2s>& getPolygon(SbBool* clip_inner=0) const;
    //@}
    
    /// Returns the screen coordinates of the origin of the path's tail object
    /*! Return value is in floating-point pixels, origin at bottom-left. */
    SbVec2f screenCoordsOfPath(SoPath *path) const;

    /** @name Edit methods */
    //@{
    void setEditing(SbBool edit);
    SbBool isEditing() const { return this->editing; }
    void setEditingCursor (const QCursor& cursor);
    void setComponentCursor(const QCursor& cursor);
    void setRedirectToSceneGraph(SbBool redirect) { this->redirected = redirect; }
    SbBool isRedirectedToSceneGraph() const { return this->redirected; }
    void setRedirectToSceneGraphEnabled(SbBool enable) { this->allowredir = enable; }
    SbBool isRedirectToSceneGraphEnabled(void) const { return this->allowredir; }
    //@}

    /** @name Pick actions */
    //@{
    // calls a PickAction on the scene graph
    bool pickPoint(const SbVec2s& pos,SbVec3f &point,SbVec3f &norm) const;
    SoPickedPoint* pickPoint(const SbVec2s& pos) const;
    const SoPickedPoint* getPickedPoint(SoEventCallback * n) const;
    SbBool pubSeekToPoint(const SbVec2s& pos);
    void pubSeekToPoint(const SbVec3f& pos);
    //@}

    /**
     * Set up a callback function \a cb which will be invoked for the given eventtype. 
     * \a userdata will be given as the first argument to the callback function. 
     */
    void addEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata = 0);
    /**
     * Unregister the given callback function \a cb.
     */
    void removeEventCallback(SoType eventtype, SoEventCallbackCB * cb, void* userdata = 0);

    /** @name Clipping plane, near and far plane */
    //@{
    /** Returns the view direction from the user's eye point in direction to the
     * viewport which is actually the negative normal of the near plane.
     * The vector is normalized to length of 1.
     */
    SbVec3f getViewDirection() const;
    void    setViewDirection(SbVec3f);
    /** Returns the up direction */
    SbVec3f getUpDirection() const;
    /** Returns the orientation of the camera. */
    SbRotation getCameraOrientation() const; 
    /** Returns the 3d point on the focal plane to the given 2d point. */
    SbVec3f getPointOnScreen(const SbVec2s&) const;
    /** Returns the near plane represented by its normal and base point. */
    void getNearPlane(SbVec3f& rcPt, SbVec3f& rcNormal) const;
    /** Returns the far plane represented by its normal and base point. */
    void getFarPlane(SbVec3f& rcPt, SbVec3f& rcNormal) const;
    /** Adds or remove a manipulator to/from the scenegraph. */
    void toggleClippingPlane();
    /** Checks whether a clipping plane is set or not. */
    bool hasClippingPlane() const;
    /** Project the given normalized 2d point onto the near plane */
    SbVec3f projectOnNearPlane(const SbVec2f&) const;
    /** Project the given normalized 2d point onto the far plane */
    SbVec3f projectOnFarPlane(const SbVec2f&) const;
    //@}
    
    /** @name Dimension controls
     * the "turn*" functions are wired up to parameter groups through view3dinventor.
     * don't call them directly. instead set the parameter groups.
     * @see TaskDimension
     */
    //@{
    void turnAllDimensionsOn();
    void turnAllDimensionsOff();
    void turn3dDimensionsOn();
    void turn3dDimensionsOff();
    void turnDeltaDimensionsOn();
    void turnDeltaDimensionsOff();
    void eraseAllDimensions();
    void addDimension3d(SoNode *node);
    void addDimensionDelta(SoNode *node);
    //@}

    /**
     * Set the camera's orientation. If isAnimationEnabled() returns
     * \a true the reorientation is animated, otherwise its directly
     * set.
     */
    void setCameraOrientation(const SbRotation& rot, SbBool moveTocenter=false);
    void setCameraType(SoType t);
    void moveCameraTo(const SbRotation& rot, const SbVec3f& pos, int steps, int ms);
    /** 
     * Zooms the viewport to the size of the bounding box. 
     */
    void boxZoom(const SbBox2s&);
    /**
     * Reposition the current camera so we can see the complete scene.
     */
    void viewAll();
    void viewAll(float factor);

    /// Breaks out a VR window for a Rift
    void viewVR(void);

    /**
     * Reposition the current camera so we can see all selected objects 
     * of the scene. Therefore we search for all SOFCSelection nodes, if
     * none of them is selected nothing happens.
     */
    void viewSelection();

    void setGradientBackground(bool b);
    bool hasGradientBackground() const;
    void setGradientBackgroundColor(const SbColor& fromColor,
                                    const SbColor& toColor);
    void setGradientBackgroundColor(const SbColor& fromColor,
                                    const SbColor& toColor,
                                    const SbColor& midColor);
    void setNavigationType(Base::Type);

    void setAxisCross(bool b);
    bool hasAxisCross(void);
    
    void setEnabledFPSCounter(bool b);
    void setEnableduseVBO(bool b);

    NavigationStyle* navigationStyle() const;

    void setDocument(Gui::Document *pcDocument);
    Gui::Document* getDocument();

    virtual PyObject *getPyObject(void);
    bool vboEnabled;
    bool get_vbo_state(); 

protected:
    void renderScene();
    void renderFramebuffer();
    void renderGLImage();
    void animatedViewAll(int steps, int ms);
    virtual void actualRedraw(void);
    virtual void setSeekMode(SbBool enable);
    virtual void afterRealizeHook(void);
    virtual bool processSoEvent(const SoEvent * ev);
    void dropEvent (QDropEvent * e);
    void dragEnterEvent (QDragEnterEvent * e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dragLeaveEvent(QDragLeaveEvent *e);
    SbBool processSoEventBase(const SoEvent * const ev);
    void printDimension();
    void selectAll();

    enum eWinGestureTuneState{
        ewgtsDisabled, //suppress tuning/re-tuning after errors
        ewgtsNeedTuning, //gestures are to be retuned upon next event
        ewgtsTuned
    };
    eWinGestureTuneState winGestureTuneState;//See ViewerEventFilter::eventFilter function for explanation

private:
    static void setViewportCB(void * userdata, SoAction * action);
    static void clearBufferCB(void * userdata, SoAction * action);
    static void setGLWidgetCB(void * userdata, SoAction * action);
    static void handleEventCB(void * userdata, SoEventCallback * n);
    static void interactionStartCB(void * data, Quarter::SoQTQuarterAdaptor * viewer);
    static void interactionFinishCB(void * data, Quarter::SoQTQuarterAdaptor * viewer);
    static void interactionLoggerCB(void * ud, SoAction* action);

private:
    static void selectCB(void * closure, SoPath * p);
    static void deselectCB(void * closure, SoPath * p);
    static SoPath * pickFilterCB(void * data, const SoPickedPoint * pick);
    void initialize();
    void drawAxisCross(void);
    static void drawArrow(void);
    void setCursorRepresentation(int mode);


private:
    std::set<ViewProvider*> _ViewProviderSet;
    std::map<SoSeparator*,ViewProvider*> _ViewProviderMap;
    std::list<GLGraphicsItem*> graphicsItems;
    ViewProvider* editViewProvider;
    SoFCBackgroundGradient *pcBackGround;
    SoSeparator * backgroundroot;
    SoSeparator * foregroundroot;
    SoDirectionalLight* backlight;

    SoSeparator * pcViewProviderRoot;
    SoEventCallback* pEventCallback;
    NavigationStyle* navigation;
    SoFCUnifiedSelection* selectionRoot;

    RenderType renderType;
    QGLFramebufferObject* framebuffer;
    QImage glImage;
    SoSwitch *dimensionRoot;

    // small axis cross in the corner
    SbBool axiscrossEnabled;
    int axiscrossSize;
    // big one in the middle
    SoShapeScale* axisCross;
    SoGroup* axisGroup;
    
    //stuff needed to draw the fps counter
    bool fpsEnabled;
    SoSeparator* fpsRoot;

    SbBool editing;
    QCursor editCursor, zoomCursor, panCursor, spinCursor;
    SbBool redirected;
    SbBool allowredir;

    std::string overrideMode;
    Gui::Document* guiDocument = nullptr;
    
    ViewerEventFilter* viewerEventFilter;
    
    PyObject *_viewerPy;

    // friends
    friend class NavigationStyle;
    friend class GLPainter;
    friend class ViewerEventFilter;
};

} // namespace Gui

#endif  // GUI_VIEW3DINVENTORVIEWER_H

