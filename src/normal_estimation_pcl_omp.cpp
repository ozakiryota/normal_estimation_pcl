#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/features/normal_3d_omp.h>

class NormalEstimationPCLOMP{
	private:
		ros::NodeHandle nh;
		ros::Subscriber sub;
		ros::Publisher pub;
		pcl::visualization::PCLVisualizer viewer{"pc_normals"};
		pcl::PointCloud<pcl::PointXYZ>::Ptr cloud {new pcl::PointCloud<pcl::PointXYZ>};
		pcl::PointCloud<pcl::PointNormal>::Ptr normals {new pcl::PointCloud<pcl::PointNormal>};

	public:
		NormalEstimationPCLOMP();
		void Callback(const sensor_msgs::PointCloud2ConstPtr& msg);
		void NormalEstimation(void);
		void Visualization(void);
		void Publication(void);
};

NormalEstimationPCLOMP::NormalEstimationPCLOMP()
{
	sub = nh.subscribe("/velodyne_points", 1, &NormalEstimationPCLOMP::Callback, this);
	pub = nh.advertise<sensor_msgs::PointCloud2>("/normals", 1);
	viewer.setBackgroundColor(1, 1, 1);
	viewer.addCoordinateSystem(0.5, "axis");
}

void NormalEstimationPCLOMP::Callback(const sensor_msgs::PointCloud2ConstPtr &msg)
{
	std::cout << "CALLBACK" << std::endl;
	pcl::fromROSMsg(*msg, *cloud);
	copyPointCloud(*cloud, *normals);
	NormalEstimation();
	Visualization();
	Publication();
}

void NormalEstimationPCLOMP::NormalEstimation(void)
{
	std::cout << "NORMAL ESTIMATION" << std::endl;
	pcl::NormalEstimationOMP<pcl::PointNormal, pcl::PointNormal> ne;
	ne.setInputCloud(normals);
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree (new pcl::search::KdTree<pcl::PointNormal> ());
	ne.setSearchMethod(tree);
	ne.setRadiusSearch(0.5);
	ne.compute(*normals);
}

void NormalEstimationPCLOMP::Visualization(void)
{
	std::cout << "VISUALIZER" << std::endl;

	viewer.removeAllPointClouds();

	viewer.addPointCloud(cloud, "cloud");
	viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_COLOR, 0.0, 0.0, 0.0, "cloud");
	viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 3, "cloud");

	viewer.addPointCloudNormals<pcl::PointNormal>(normals, 10, 0.5, "normals");
	viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 0.0, 0.0, "normals");
	viewer.setPointCloudRenderingProperties (pcl::visualization::PCL_VISUALIZER_LINE_WIDTH, 3, "normals");

	viewer.spinOnce();
}

void NormalEstimationPCLOMP::Publication(void)
{
	sensor_msgs::PointCloud2 normals_pub;
	pcl::toROSMsg(*normals, normals_pub);
	pub.publish(normals_pub);
}

int main(int argc, char** argv)
{
	ros::init(argc, argv, "normal_estimation_pcl_omp");
	std::cout << "Normal Estimation PCL OMP" << std::endl;

	NormalEstimationPCLOMP normal_estimation_pcl_omp;
	ros::spin();
}
