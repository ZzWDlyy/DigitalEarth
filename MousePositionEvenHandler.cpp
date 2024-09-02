#include "MousePositionEvenHandler.h"

Cv::EventHandlers::MousePositionEvenHandler::MousePositionEvenHandler(osgEarth::MapNode* node, osgEarth::Util::Controls::LabelControl* control)
{
	this->mapNode = node;
	this->label = control;
}

bool Cv::EventHandlers::MousePositionEvenHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{
	osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);

	if (viewer)
	{
		std::string positionStr;
		//鼠标移动
		if (ea.getEventType() == ea.MOVE)
		{
			osg::Vec3d world;
			if (mapNode->getTerrain()->getWorldCoordsUnderMouse(aa.asView(), ea.getX(), ea.getY(), world))
			{
				osgEarth::GeoPoint pt;
				pt.fromWorld(mapNode->getMapSRS(), world);
				positionStr.append("  longitude:");
				positionStr.append(std::to_string(pt.x()));
				positionStr.append("  latitude:");
				positionStr.append(std::to_string(pt.y()));
				/*positionStr.append("  dem:");
				positionStr.append(std::to_string(pt.z()));*/
			}

			osgUtil::LineSegmentIntersector::Intersections hits;
			/*if (viewer->computeIntersections(ea.getX(), ea.getY(), hits))
			{
				auto first = hits.begin()->getWorldIntersectPoint();
				osgEarth::GeoPoint pt;
				pt.fromWorld(mapNode->getMapSRS(), first);
				positionStr.append("  交点经度:");
				positionStr.append(std::to_string(pt.x()));
				positionStr.append("  交点纬度:");
				positionStr.append(std::to_string(pt.y()));
				positionStr.append("  交点高程:");
				positionStr.append(std::to_string(pt.z()));
			}*/
			this->label->setText(positionStr);
		}
	}
	return false;
}