#include "kdtree.h"

template<class T>
class ofxKdTree
{
public:
	struct kdtree *kd;
	
	ofxKdTree()
	{
		kd = kd_create(3);
	}
	
	virtual ~ofxKdTree()
	{
		kd_free(kd);
	}
	
	void clear()
	{
		kd_clear(kd);
	}
	
	void insert(const ofPoint& p)
	{
		insert(p.x, p.y, p.z, 0);
	}
	
	void insert(float x, float y, float z, T *data)
	{
		kd_insert3f(kd, x, y, z, data);
	}
	
	T* getNearest(const ofPoint& p)
	{
		return getNearest(p.x, p.y, p.z);
	}
	
	T* getNearest(float x, float y, float z)
	{
		kdres *res = kd_nearest3f(kd, x, y, z);
		double pos[3];
		kd_res_item( res, pos );
		T *result = new T(pos[0], pos[1], pos[2]);
		kd_res_free(res);
		return result;
	}
	
	vector<T*> getNearestRange(const ofPoint& p, float range, bool sort_result = false)
	{
		return getNearestRange(p.x, p.y, p.z, sort_result);
	}
	
	vector<T*> getNearestRange(float x, float y, float z, float range, bool sort_result = false)
	{
		kdres *res = kd_nearest_range3f(kd, x, y, z, range);
		vector<T*> result;
		
		if (sort_result)
		{
			const int num_res = kd_res_size(res);
			while (object_position_array.size() < num_res)
			{
				object_position *o = new object_position();
				object_position_array.push_back(o);
			}
			
			int index = 0;
			while (!kd_res_end(res))
			{
				object_position *o = object_position_array[index];
				o->item = (T*)kd_res_itemf(res, o->pos);
				
				kd_res_next(res);
				index++;
			}
			
			sort(object_position_array.begin(), object_position_array.begin() + num_res, sort_by_distance(x, y, z));
			
			
			for (int i = 0; i < num_res; i++)
			{
				object_position *o = object_position_array[i];
				result.push_back(o->item);
			}
		}
		else
		{
			while (!kd_res_end(res))
			{
				double pos[3];
				kd_res_item( res, pos );
				T *resultingT = new T(pos[0], pos[1], pos[2]);
				result.push_back(resultingT);
				kd_res_next( res );
			}
		}
		
		kd_res_free(res);
		
		return result;
	}
	
private:
	
	struct object_position
	{
		T *item;
		float pos[3];
	};
	vector<object_position*> object_position_array;
	
	struct sort_by_distance
	{
		ofVec3f pos;
		
		sort_by_distance(float xx, float yy, float zz) : pos(xx, yy, zz)
		{
		}
		
		bool operator()(object_position *a, object_position *b)
		{
			float x1 = a->pos[0], y1 = a->pos[1], z1 = a->pos[2];
			float x2 = b->pos[0], y2 = b->pos[1], z2 = b->pos[2];
			
			x1 -= pos.x;
			y1 -= pos.y;
			z1 -= pos.z;
			
			x2 -= pos.x;
			y2 -= pos.y;
			z2 -= pos.z;
			
			x1 = x1*x1;
			y1 = y1*y1;
			z1 = z1*z1;
			
			x2 = x2*x2;
			y2 = y2*y2;
			z2 = z2*z2;
			
			return (x1 + y1 + z1) < (x2 + y2 + z2);
		}
	};
};