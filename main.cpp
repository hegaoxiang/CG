#include<iostream>
#include<math.h>
#include<array>
#include <vector>
#include <map>
#include <list>
#include <list>
#include <stack>
//#include <windef.h>


using namespace std;
const int SCREEN_X = 80;
const int SCREEN_Y = 60;
array<array<int, SCREEN_Y >, SCREEN_X> canvas;


void SetPixel(int x, int y,int color = 1)
{
	canvas[x][y] = color;
}
void Print(array<array<int, SCREEN_Y >, SCREEN_X> &canvas)
{
	for (int y = SCREEN_Y - 1; y >= 0; y--)
	{
		for (size_t x = 0; x < SCREEN_X; x++)
		{
			cout << canvas[x][y];
		}
		cout << endl;
	}
}

void DrawLine_Bresenham(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	int flagX = (dx > 0) ? 1 : -1;
	int flagY = (dy > 0) ? 1 : -1;

	dx *= flagX;
	dy *= flagY;

	int step = (dx > dy) ? dx : dy;

	int e = 0;// e = d * dx
	int x = 0, y = 0;// 将直线平移到x1，y1为原点，通过flagX和flagY解决非第一象限的直线问题

	if (dx > dy)
	{
		for (int i = 0; i < step; i++)
		{
			SetPixel(x * flagX + x1, y * flagY + y1);
			x++;

			e = e + dy;

			if (2 * e > dx)
			{
				e -= dx;
				y++;
			}
		}
	}
	else
	{
		for (int i = 0; i < step; i++)
		{
			SetPixel(x * flagX + x1, y * flagY + y1);
			y++;

			e = e + dx;

			if (2 * e > dy)
			{
				e -= dy;
				x++;
			}

		}
	}
}

void DrawLine_DDA(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;

	int step = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

	float xIncre = (float)dx / (float)step;
	float yIncre = (float)dy / (float)step;

	float x = x1, y = y1;

	for (int i = 0; i < step; i++)
	{
		SetPixel((int)(x + 0.5), (int)(y + 0.5));
		x += xIncre;
		y += yIncre;
	}
}
void DrawCircle(int x, int y, int radius)
{
	int x0 = 0;
	int y0 = radius;

	int d = 1 - radius;
	while (x0 <= y0)
	{
		SetPixel(x + x0, y + y0);
		SetPixel(x + x0, y + -y0);
		SetPixel(x + -x0, y + y0);
		SetPixel(x + -x0, y + -y0);
		SetPixel(x + y0, y + x0);
		SetPixel(x + y0, y + -x0);
		SetPixel(x + -y0, y + x0);
		SetPixel(x + -y0, y + -x0);

		if (d < 0)
			d += 2 * x0 + 3;
		else
		{
			d += 2 * (x0 - y0) + 5;
			y0--;
		}
		x0++;
	}
}

void DrawEllipse(int x, int y, int a, int b)
{
	int a2 = a * a;
	int b2 = b * b;

	int x0 = 0;
	int y0 = b;

	int d = 4 * b2 + a2 - 4 * a2 * b;

	while (y0 >= 0)
	{
		if (2 * b2 * x0 < 2 * a2 * y0)
		{
			x0++;
			if (d > 0)
			{
				d += 4 * ((2 * x0 + 3) * b2 + (1 - 2 * y0) * a2);
				y0--;
			}
			else
			{
				d += 4 * b2 * (2 * x0 + 3);
			}
		}
		else
		{
			y0--;
			if (d > 0)
			{
				d += 4 * a2 * (-2 * y0 + 3);
			}
			else
			{
				d += 4 * (b2 * (2 * x0 + 2) + a2 * (-2 * y0 + 3));
				x0++;
			}
		}

		SetPixel(x + x0, y + y0);
		SetPixel(x + x0, y - y0);
		SetPixel(x - x0, y + y0);
		SetPixel(x - x0, y - y0);
	}
}

typedef struct tagPOINT
{
	int  x;
	int  y;

	bool operator < (const tagPOINT& p)const
	{
		return x < p.x;
	}
} POINT;

const float GetK(const POINT& p1,const POINT& p2)
{
	int dy = p1.y - p2.y;
	int dx = p1.x - p2.x;
	return  static_cast<float>(dx) / static_cast<float>(dy);
}
void DrawPolygon(vector<POINT> &points)
{
	typedef struct Edge {
		POINT p1;
		POINT p2;

		int  GetXAxisValue(int &x,int y)
		{
			double b = static_cast<double> (y - p1.y);
			double c = static_cast<double> (p2.x - p1.x);
			double d = static_cast<double> (p2.y - p1.y);
			double tempX = (b * c / d + p1.x);
			x = static_cast<int>(tempX + 0.5);
			// On the Line Segment
			if ((min(p1.x, p2.x) < tempX && tempX < max(p1.x, p2.x)))
				return 1;
			// Endpoint of line segment
			if ((min(p1.x, p2.x) == tempX || tempX == max(p1.x, p2.x)))
				return 0;
			// not on the Line Segment;
			return -1;
		}
	}Edge;

	vector<Edge> edges;
	// 得到Ymin和Ymax
	int minY = 2000;
	int maxY = 0;
	for (auto item : points)
	{
		minY = (item.y >= minY) ? minY : item.y;
		maxY = (item.y <= maxY) ? maxY : item.y;
	}

	// 得到每个顶点的额外取舍信息
	map<POINT, int> info;
	vector<int> infoSub(points.size());
	auto iter_info = infoSub.begin();
	for (auto iter = points.begin(); iter != points.end() - 1; ++iter,++iter_info)
	{		POINT& p1 = *iter;
		POINT& p2 = *(iter + 1);

		// 添加顶点信息
		if (p1.y > p2.y)
			*(iter_info + 1) += 1;
		else if (p1.y < p2.y)
			*iter_info += 1;
		// 添加边
		edges.push_back(Edge{p1,p2});
	}
	// 处理最后一个顶点
	POINT& p1 = *points.begin();
	POINT& p2 = *(points.end() - 1);
	// 添加顶点信息
	if (p1.y > p2.y)
		* (infoSub.end() - 1) += 1;
	else if (p1.y < p2.y)
		* infoSub.begin() += 1;
	// 添加边
	edges.push_back(Edge{ p1,p2 });

	// 放入map中
	for (size_t i = 0; i < infoSub.size(); i++)
	{
		auto p = points[i];
		int temp = infoSub[i];
		info.insert(map<POINT, int>::value_type(POINT(p), temp));
	}
	for (int i = minY; i < maxY; i++)
	{
		vector<int> intersections;
		// 求交
		for (size_t j = 0; j < edges.size(); j++)
		{
			Edge& edge = edges[j];
			// Find the intersection 
			int intersection;
			if (edge.GetXAxisValue(intersection, i) == 1)
			{
				intersections.push_back(intersection);
			}
			else if(edge.GetXAxisValue(intersection, i) == 0)
			{
				if (info.at(POINT{ intersection,i }) != 0)
				{
					intersections.push_back(intersection);
					info.at(POINT{ intersection,i }) -= 1;
				}
					
			}
		}
		// 排序
		sort(intersections.begin(), intersections.end());
		// 配对
		for (auto iter = intersections.begin(); iter != intersections.end(); iter += 2)
		{
			auto x1 = *iter;
			auto x2 = *(iter + 1);
			for (int j = x1; j < x2; j++)
			{
				SetPixel(j, i);
			}
		}
		// 填色
	}
}

void test(vector<POINT> &points)
{
	typedef struct EdgeNode
	{
		float xLow;
		int yHigh;
		float k;
		EdgeNode* next;
	}EdgeNode;

	// 得到Ymin和Ymax
	int minY = 2000;
	int maxY = 0;
	for (auto item : points)
	{
		minY = (item.y >= minY) ? minY : item.y;
		maxY = (item.y <= maxY) ? maxY : item.y;
	}
	int edgeTableSize = maxY - minY + 1;

	vector<EdgeNode> edgeTable(edgeTableSize);
	
	
	for (auto iter = points.begin(); iter != points.end() ; ++iter)
	{
		EdgeNode* temp = new EdgeNode();
		POINT& p1 = *iter;
		POINT& p2 = (iter != points.end() - 1)?(*(iter + 1)):(*points.begin());

		EdgeNode* currentNode;
		// 添加顶点信息
		if (p1.y > p2.y)
		{
			currentNode = &edgeTable[p2.y - minY];

			temp->xLow = p2.x;
			temp->yHigh = p1.y - 1;
			temp->k = GetK(p1, p2);
			temp->next = nullptr;
		}
		else 
		{
			currentNode = &edgeTable[p1.y - minY];

			temp->xLow = p1.x;
			temp->yHigh = p2.y - 1;
			temp->k = GetK(p1, p2);
			temp->next = nullptr;
		}
		if (currentNode->next == NULL)
		{
			//currentNode->xLow = 1;
			currentNode->next = temp;
		}
		else
		{
			auto nextNode = currentNode->next;
			// join and sort
			while (nextNode != NULL)
			{
				if (temp->xLow < nextNode->xLow || (temp->xLow == nextNode->xLow && temp->k < nextNode->k))
				{
					// 把新点加入到nextNode前
					currentNode->next = temp;
					temp->next = nextNode;
					break;
				}
				else
				{
					currentNode = nextNode;
					nextNode = nextNode->next;
				}
			}
			if (temp->next == NULL)
			{
				currentNode->next = temp;
			}
		}
	};

	list<EdgeNode> AET;

	for (int i = 0; i < edgeTableSize; i++)
	{
		int currentY = i + minY;
		auto currentNode = edgeTable[i].next;
		// 加入新边
		while (currentNode != nullptr)
		{
			auto& ae = *currentNode;
			// 插入新边
			auto iter = AET.begin();
			for (; iter != AET.end(); ++iter)
			{
				if (iter->xLow > ae.xLow)
				{
					if (iter == AET.begin())// 插入第一个
					{
						AET.push_front(ae);
					}
					else
					{
						
						AET.insert(iter, ae);
						
					}
					break;
				}
			}

			if (iter == AET.end())
			{
				AET.push_back(ae);
			}
			currentNode = currentNode->next;
		}
		// 遍历AET，上色后处理删除事件
		for (auto iter = AET.begin(); iter != AET.end() ; ++iter,++iter)
		{
			if (++iter == AET.end())
			{
				break;
			}
			--iter;
			// 上色
			auto iter_back = iter;
			
			iter_back++;

			auto &x1 = (*iter);
			auto &x2 = (*iter_back);
			
			for (int x = x1.xLow + 0.5; x <= x2.xLow; x++)
			{
				SetPixel(x, currentY);
			}
			// 处理删除

			x1.xLow += x1.k;
			x2.xLow += x2.k;
		}

		for (auto iter = AET.begin(); iter != AET.end(); )
		{
			
			if (iter->yHigh == currentY)
			{
				iter = AET.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	
	}                             
	return;
}
int GetPixelColor(POINT& p)
{
	return canvas[p.x][p.y];
}
void Seed(POINT& p, int boundColor, int color)
{
	stack<POINT> points;
	points.push(p);

	while (!points.empty())
	{
		auto currentPoint = points.top();
		if (GetPixelColor(currentPoint) == boundColor || GetPixelColor(currentPoint) == color)
		{
			points.pop();
			continue;
		}
		
		SetPixel(currentPoint.x, currentPoint.y, color);
		points.push(POINT{ currentPoint.x - 1,currentPoint.y });
		points.push(POINT{ currentPoint.x + 1,currentPoint.y });
		points.push(POINT{ currentPoint.x ,currentPoint.y + 1 });
		points.push(POINT{ currentPoint.x ,currentPoint.y - 1 });
		
	}
}
int main()
{
	//Print(canvas);
	//DrawLine_DDA(0, 0, 10,40);
	//DrawLine_DDA(10, 40, 0, 0);
	//DrawLine_Bresenham(10, 40, 0, 0);
	//DrawLine_Bresenham(0, 0, 10, 40);;
	//DrawCircle(40,40, 10);
	//Print(canvas);
	//vector<POINT> points;
	//for (int i = 0; i < 7; i++)
	//{
	//	POINT s;
	//	cin >> s.x >> s.y;
	//	points.push_back(s);
	//}
	//
	//test(points);
	DrawLine_Bresenham(0, 0, 20, 0);
	DrawLine_Bresenham(0, 0, 10, 10);
	DrawLine_Bresenham(10, 10, 20, 0);
	POINT s;
	s.x = 10;
	s.y = 2;
	Seed(s, 1, 2);
	Print(canvas);
	return 0;
}
