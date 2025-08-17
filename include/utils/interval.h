#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
	public:
		double min, max;
		
		// Empty by default
		interval() : min(+inf), max(-inf) {}
		
		interval(double min, double max) : min(min), max(max) {}
		
		interval(const interval& a, const interval& b) {
			min = (a.min <= b.min) ? a.min : b.min;
			max = (a.max >= b.max) ? a.max : b.max;
		}
		
		double size() const {return max - min;}
		
		// x ∈ [min, max]
		bool has_closed(double x) const {
			return min <= x && x <= max;
		}
		
		// x ∈ ]min, max[
		bool has_open(double x) const {
			return min < x && x < max;
		}
		
		double clamp(double x) const {
			if(x < min) return min;
			if(x > max) return max;
			return x;
		}
		
		interval expand(double delta) const {
			return interval(min - delta/2, max + delta/2);
		}
		
		static const interval empty, universe, positive;
};

#endif