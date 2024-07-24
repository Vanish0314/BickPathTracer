/*
 * @Author: Vanish
 * @Date: 2024-05-31 03:57:21
 * @LastEditTime: 2024-07-14 12:56:38
 * Also View: http://vanishing.cc
 * Copyright@ https://creativecommons.org/licenses/by/4.0/deed.zh-hans
 */
#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <cmath>

#include "Bick.h"
#include "Scene.h"
#include "Ray.h"
#include "Color.h"
#include "Random.h"
#include "Debuger.h"

class Camera
{
public:
	Vector3  m_position;
	Vector3  m_direction;
	Vector3  m_up;

	uint16_t imageWidth, imageHeight;
	float    fov;// 垂直FOV

public:
	Camera(Vector3 position, Vector3 direction, Vector3 up,int imageWidth, int imageHeight, float fov)
		: m_position(position), m_direction(direction), m_up(up), imageWidth(imageWidth), imageHeight(imageHeight), fov(fov), samplesPerPixel(SAMPLE_COUNT)	
		{
			// 计算摄像机画布
			double theta = fov; // 视角角度(水平)
			double halfTheta = theta / 2.0; // 视角角度的一半
			halfTheta = halfTheta * PI / 180.0; // 转换为弧度
			double viewportWidth =  2.0 * nearZ * std::tan(halfTheta); // 视角宽度
			double ratio = (double)imageHeight / (double)imageWidth; // 画布宽高比
			double viewportHeight = viewportWidth * ratio; // 视角高度

			//计算摄像机画布坐标系
			w = m_direction; // 视线方向
			u = m_up.Cross(w).Normalized(); // 右侧方向
			v = Vector3(0,0,0) - m_up; // 下侧方向

			//计算视角坐标系原点
			Vector3 x = u * (viewportWidth / 2.0);
			Vector3 y = m_up * (viewportHeight / 2.0);
			Vector3 z = m_direction * nearZ;
		    pixel_00_Location = m_position - x + y +z;

			pixelDeltaX = u * (viewportWidth / (double)imageWidth); // 视角坐标系每一个像素x方向的大小
			pixelDeltaY = v * (viewportHeight / (double)imageHeight); // 视角坐标系每一个像素y方向的大小

		}
	~Camera() {}

public:
	void Render(const Scene& scene,std::ostream& output);

private:
	void WriteFileHeader(std::ostream& output);
	void GetRay(int pixelIndexX,int pixelIndexY,Ray& ray);
	Color RadianceToColor(Vector3 radiance);
	void WritePixelColor(std::ostream& output,Color& color);

private:
	Vector3 pixel_00_Location;// 视角坐标系原点，位于画布左上角,00像素的中心位置
	Vector3 u,v,w; // 视角坐标系
	Vector3 pixelDeltaX, pixelDeltaY; // 视角坐标系每一个像素的大小
	double nearZ = 1.0; // 近裁剪面
	int samplesPerPixel = SAMPLE_COUNT; // 采样数

public:
	

};