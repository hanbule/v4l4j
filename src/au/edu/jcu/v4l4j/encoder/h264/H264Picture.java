package au.edu.jcu.v4l4j.encoder.h264;

import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.io.Closeable;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

import au.edu.jcu.v4l4j.FrameGrabber;
import au.edu.jcu.v4l4j.V4L4JUtils;
import au.edu.jcu.v4l4j.VideoFrame;
import au.edu.jcu.v4l4j.exceptions.UnsupportedMethod;

/**
 * Wrapper for <code>x264_picture_t</code>.
 * @author mailmindlin
 */
public class H264Picture implements Closeable, VideoFrame {
	
	static {
		V4L4JUtils.loadLibrary();
	}
	
	protected static native long alloc();
	
	private static native long init(int width, int height, int csp);
	
	/**
	 * Sets PTS value iff parameter is positive (>=0).
	 * Returns final PTS value.
	 * @param object Pointer to x264_picture_t
	 * @param pts new PTS value to set
	 * @return PTS value
	 */
	private static native int doSetAndGetPts(long object, int pts);
	
	protected static native int doGetStride(long ptr, int plane);
	
	protected static native int doGetNumPlanes(long ptr);
	
	private static native ByteBuffer doGetBuffer(long ptr, int plane);
	
	private static native void putInPlane(long ptr, int plane, ByteBuffer buf);
	
	private static native void putImage(long ptr, int imf, ByteBuffer buf);
	
	public static H264Picture allocate(int width, int height, int csp) {
		return new H264Picture(init(width, height, csp), width, height, csp);
	}
	
	@SuppressWarnings("unchecked")
	protected WeakReference<ByteBuffer>[] buffers = (WeakReference<ByteBuffer>[])new WeakReference[4];
	protected final long object;
	protected final int csp;
	protected final int width;
	protected final int height;
	
	/**
	 * Release the native memory behind this picture
	 */
	@Override
	public native void close();
	
	/**
	 * Initialize picture with given pointer
	 * @param pointer address of struct
	 */
	protected H264Picture(long pointer) {
		this.object = pointer;
		this.width = 0;
		this.height = 0;
		this.csp = 0;
	}
	
	protected H264Picture(long pointer, int width, int height, int csp) {
		this.object = pointer;
		this.width = width;
		this.height = height;
		this.csp = csp;
	}
	
	/**
	 * Initialize with given width, height, and CSP
	 * @param csp color space
	 * @param width width
	 * @param height height
	 */
	public H264Picture(int width, int height, int csp) {
		this.csp = csp;
		this.width = width;
		this.height = height;
		this.object = init(csp, width, height);
	}
	
	public H264Picture() {
		this(alloc());
	}
	
	/**
	 * Get color space
	 * @return csp
	 */
	public int getCsp() {
		return csp;
	}
	/**
	 * Get width of picture
	 * @return width
	 */
	public int getWidth() {
		return width;
	}
	/**
	 * Get height of picture
	 * @return height
	 */
	public int getHeight() {
		return height;
	}
	
	void setPts(int pts) {
		if (pts < 0)
			throw new IllegalArgumentException("PTS must be positive (was " + pts + ")");
		doSetAndGetPts(this.object, pts);
	}

	@Override
	public FrameGrabber getFrameGrabber() {
		return null;
	}

	@Override
	public int getFrameLength() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getSequenceNumber() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getCaptureTime() {
		// TODO Auto-generated method stub
		return 0;
	}
	

	@Override
	public ByteBuffer getBuffer() {
		return null;
	}

	@Override
	public DataBuffer getDataBuffer() {
		return new DataBuffer(DataBuffer.TYPE_UNDEFINED, 0, 4) {
			@Override
			public int getElem(int bank, int i) {
				ByteBuffer buf = getBuffer(bank);
				return buf.getInt(i);
			}

			@Override
			public void setElem(int bank, int i, int val) {
				ByteBuffer buf = getBuffer(bank);
				buf.putInt(i, val);
			}
		};
	}

	@Override
	public Raster getRaster() throws UnsupportedMethod {
		throw new UnsupportedMethod("Cannot get raster for H264 image");
	}

	@Override
	public BufferedImage getBufferedImage() throws UnsupportedMethod {
		throw new UnsupportedMethod("Cannot get BufferedImage for H264 image");
	}

	@Override
	public void recycle() {
		// TODO Auto-generated method stub
		
	}

	public ByteBuffer getBuffer(int plane) {
		ByteBuffer result;
		WeakReference<ByteBuffer> bufferRef = this.buffers[plane];
		if (bufferRef == null || (result = bufferRef.get()) == null) {
			synchronized (this.buffers) {
				//Check again, now that we have a lock on the buffers
				if (bufferRef == null || (result = bufferRef.get()) == null)
					this.buffers[plane] = new WeakReference<>(result = doGetBuffer(this.object, plane));
			}
		}
		return result.duplicate();
	}
}
