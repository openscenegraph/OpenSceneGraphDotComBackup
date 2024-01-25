class UpdateViewportAndFBOAfterTextureResizeCallback :public osg::NodeCallback
{
public:
  UpdateViewportAndFBOAfterTextureResizeCallback(bool dirty = false) : _dirty(dirty) {}

  void setDirty(bool dirty) { _dirty = dirty; }
  bool getDirty() { return _dirty; }

  void operator()(osg::Node *node, osg::NodeVisitor *nv)
  {
    if (_dirty)
    {
      osgUtil::CullVisitor *cv = static_cast<osgUtil::CullVisitor *>(nv);
      if (cv && node == cv->getCurrentRenderStage()->getCamera())
      {
        cv->getCurrentRenderStage()->setCameraRequiresSetUp(true);
        _dirty = false;
      }
    }

    traverse(node, nv);
  }
protected:
  bool _dirty;
};

class UpdatePreRenderCameraSize : public osg::NodeCallback
{
  osg::Camera* _preRenderCamera; 

public:
  UpdatePreRenderCameraSize(osg::Camera* camera) : _preRenderCamera(camera)  {}

  void operator()(osg::Node *node, osg::NodeVisitor *nv)
  {
    osg::Camera * camera = _preRenderCamera;

    if (nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
      osgUtil::CullVisitor *cv = static_cast<osgUtil::CullVisitor *>(nv);

      osg::Camera * rootCamera = cv->getRenderStage()->getCamera();

      osg::Matrix view(rootCamera->getViewMatrix());
      osg::Matrix projection(rootCamera->getProjectionMatrix());

      // grab the context of the parent window
      osg::GraphicsContext * gc = rootCamera->getGraphicsContext();

      // read the root window size to have some reasonable fallback in case viewport was NULL
      int x = 0, y = 0, cx = gc->getTraits()->width, cy = gc->getTraits()->height;

      osg::Viewport * vp = rootCamera->getViewport();

      if (vp)
      {
        x = vp->x();
        y = vp->y();
        cx = vp->width();
        cy = vp->height();
      }

      osg::Texture2D* color =
        dynamic_cast<osg::Texture2D *>(camera->getBufferAttachmentMap()[osg::Camera::COLOR_BUFFER]._texture.get());

      // adjust viewport and force FBO update if texture size was changed in UpdatePRCamCallback
      if (color && (color->getTextureHeight() != cy || color->getTextureWidth() != cx))
      {
        color->setTextureSize(cx, cy);
        color->dirtyTextureObject();

        UpdateViewportAndFBOAfterTextureResizeCallback* refreshFboCb =
          dynamic_cast<UpdateViewportAndFBOAfterTextureResizeCallback*>(camera->getCullCallback());

        if (refreshFboCb)
          refreshFboCb->setDirty(true);
      }

      camera->setViewport(0, 0, cx, cy);
      camera->setViewMatrix(view);
      camera->setProjectionMatrix(projection);
    }
    this->traverse(node, nv);
  }
};

void SetupPreRenderCamera( osg::Group * mainCameraRoot /* could be any group node under main camera, probably main camera too (but have not tested it) */ )
{
  int width = 1920, height = 1080;

  osg::ref_ptr<osg::Camera> PreRenderCamera = new osg::Camera;
  PreRenderCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  PreRenderCamera->setRenderOrder(osg::Camera::PRE_RENDER, 1);
  PreRenderCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
  PreRenderCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
  PreRenderCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  PreRenderCamera->setViewport(0, 0, width, height);
  PreRenderCamera->getOrCreateStateSet()->setAttributeAndModes(
    new osg::BlendFunc(osg::BlendFunc::ONE,
    osg::BlendFunc::ZERO),
    osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

  PreRenderCamera->setCullCallback(new UpdateViewportAndFBOAfterTextureResizeCallback);

  // Insert pre render group used to invoke UpdatePreRenderCameraCallback when Main camera window resolution changes
  osg::ref_ptr<osg::Group>  PreRenderGroup = new osg::Group;
  PreRenderGroup->setCullCallback(new UpdatePreRenderCameraSizeCallback(PreRenderCamera));

  // Insert as first child to be sure its traversed as first node 
  mainCameraRoot->insertChild(0, PreRenderGroup);
}
