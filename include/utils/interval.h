#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
	public:
		double min, max;
		
		// Empty by default
		interval() : min(+inf), max(-inf) {}
		
		interval(double min, double max) : min(min), max(max) {}
		
		double size() const {return max - min;}
		
		// x ∈ [min, max]
		bool has_closed(double x) const {
			return min <= x && x <= max;
		}
		
		// x ∈ ]min, max[
		bool has_open(double x) const {
			return min < x && x < max;
		}
		
		static const interval empty, universe, positive;
};

#endif