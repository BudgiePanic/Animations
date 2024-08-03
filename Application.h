#pragma once
#ifndef _H_APPLICATION_
#define _H_APPLICATION_

// Animation viewer application
class Application {
private:
	// private copy constructor :: to prevent multiple objects pointing to the same external resources
	Application(const Application&);
	// reduce visibility of = operator for this class :: to prevent multiple objects pointing to the same external resources
	Application& operator= (const Application&);
public:
	inline Application() {}
	inline virtual ~Application() {} // virtual destructor
//	Application Life Cycle Methods
	// Initialize application resources
	inline virtual void Initialize() {}
	// Release application resources
	inline virtual void ShutDown() {}
	/**
	 * @param deltaTime
	 *   The time since the last update in milliseconds
	 */
	inline virtual void Update(float deltaTime) {}
	/**
	 * @param aspectRatio
	 *   The aspect ratio of the rendering window
 	 */
	inline virtual void Render(float aspectRatio) {}
};

#endif