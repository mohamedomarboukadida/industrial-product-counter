import cv2
import urllib.request
import numpy as np
import time

# ===== CONFIGURATION =====
# Replace with your ESP32-CAM's IP address (shown in Serial Monitor)
ESP32_IP = "192.168.1.61"
CAMERA_URL = f'http://{ESP32_IP}/cam-lo.jpg'

# Detection parameters (adjust based on your setup)
MIN_CONTOUR_AREA = 500      # Minimum object size to detect
MAX_CONTOUR_AREA = 50000    # Maximum object size to detect
BLUR_KERNEL = (11, 11)      # Gaussian blur kernel size
CANNY_LOW = 30              # Canny edge detection low threshold
CANNY_HIGH = 150            # Canny edge detection high threshold
DILATION_ITERATIONS = 2     # Number of dilation iterations

# ===== INITIALIZE WINDOWS =====
cv2.namedWindow("Live Feed", cv2. WINDOW_AUTOSIZE)
cv2. namedWindow("Edge Detection", cv2. WINDOW_AUTOSIZE)

def get_frame(url):
    """Fetch frame from ESP32-CAM with error handling"""
    try:
        img_resp = urllib. request.urlopen(url, timeout=5)
        img_np = np.array(bytearray(img_resp.read()), dtype=np. uint8)
        frame = cv2.imdecode(img_np, -1)
        return frame
    except Exception as e:
        print(f"Error fetching frame: {e}")
        return None

def process_frame(frame):
    """Process frame and detect objects"""
    # Convert to grayscale
    gray = cv2. cvtColor(frame, cv2.COLOR_BGR2GRAY)
    
    # Apply Gaussian blur to reduce noise
    blurred = cv2.GaussianBlur(gray, BLUR_KERNEL, 0)
    
    # Apply Canny edge detection
    edges = cv2. Canny(blurred, CANNY_LOW, CANNY_HIGH)
    
    # Dilate edges to close gaps (FIXED: proper kernel)
    kernel = np.ones((3, 3), np.uint8)
    dilated = cv2.dilate(edges, kernel, iterations=DILATION_ITERATIONS)
    
    # Find contours
    contours, _ = cv2.findContours(dilated.copy(), cv2. RETR_EXTERNAL, cv2. CHAIN_APPROX_SIMPLE)
    
    # Filter contours by area
    valid_contours = []
    for contour in contours:
        area = cv2.contourArea(contour)
        if MIN_CONTOUR_AREA < area < MAX_CONTOUR_AREA:
            valid_contours.append(contour)
    
    return dilated, valid_contours

def draw_results(frame, contours, count):
    """Draw contours and count on frame"""
    result = frame.copy()
    
    # Draw contours
    cv2.drawContours(result, contours, -1, (0, 255, 0), 2)
    
    # Draw bounding boxes and numbers for each object
    for i, contour in enumerate(contours):
        x, y, w, h = cv2.boundingRect(contour)
        cv2.rectangle(result, (x, y), (x + w, y + h), (255, 0, 0), 2)
        cv2.putText(result, f"#{i+1}", (x, y - 10), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)
    
    # Display count on frame
    cv2.putText(result, f"Objects Detected: {count}", (10, 30),
                cv2. FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    
    # Display instructions
    cv2. putText(result, "Press 'q' to quit | 's' to save snapshot", (10, frame.shape[0] - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
    
    return result

def main():
    print("=" * 50)
    print("Industrial Product Counter")
    print("=" * 50)
    print(f"Connecting to ESP32-CAM at: {CAMERA_URL}")
    print("Press 'q' to quit")
    print("Press 's' to save a snapshot")
    print("=" * 50)
    
    total_count = 0
    frame_count = 0
    start_time = time. time()
    
    while True:
        # Get frame from ESP32-CAM
        frame = get_frame(CAMERA_URL)
        
        if frame is None:
            print("Waiting for camera connection...")
            time. sleep(1)
            continue
        
        # Process frame
        edges, contours = process_frame(frame)
        current_count = len(contours)
        
        # Draw results
        result_frame = draw_results(frame, contours, current_count)
        
        # Calculate FPS
        frame_count += 1
        elapsed_time = time.time() - start_time
        fps = frame_count / elapsed_time if elapsed_time > 0 else 0
        cv2.putText(result_frame, f"FPS: {fps:.1f}", (frame. shape[1] - 100, 30),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 2)
        
        # Display windows
        cv2. imshow("Live Feed", result_frame)
        cv2.imshow("Edge Detection", edges)
        
        # Handle keyboard input
        key = cv2.waitKey(1) & 0xFF
        
        if key == ord('q'):
            print(f"\nFinal Count: {current_count} objects")
            break
        elif key == ord('s'):
            # Save snapshot
            timestamp = time.strftime("%Y%m%d_%H%M%S")
            cv2.imwrite(f"snapshot_{timestamp}.jpg", result_frame)
            print(f"Snapshot saved: snapshot_{timestamp}.jpg")
    
    cv2. destroyAllWindows()
    print("Program ended.")

if __name__ == "__main__":
    main()