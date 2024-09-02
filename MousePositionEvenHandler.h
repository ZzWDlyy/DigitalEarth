#pragma once
#ifndef MOUSEPOSITIONEVENTHANDLER_H
#define MOUSEPOSITIONEVENTHANDLER_H

#include <osgGA/GUIEventHandler>
#include <osgEarth/Terrain>
#include <osgEarth/MapNode>
#include <osgEarth/Controls>
#include <osgViewer/Viewer>

namespace Cv {
    namespace EventHandlers {

        class MousePositionEvenHandler : public osgGA::GUIEventHandler {
        public:
            MousePositionEvenHandler(osgEarth::MapNode* node, osgEarth::Util::Controls::LabelControl* control);

            virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

        private:
            osgEarth::MapNode* mapNode;
            osgEarth::Util::Controls::LabelControl* label;
        };

    } // namespace EventHandlers
} // namespace Cv

#endif // MOUSEPOSITIONEVENTHANDLER_H