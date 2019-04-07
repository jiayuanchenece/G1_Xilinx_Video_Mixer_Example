#include "DisplayControl/display_ctrl.h"
#include "InterruptControl/intc.h"
#include "platform.h"
#include "xil_cache.h"

void DemoInitialize();
void DemoPrintTest(u8 *frame, u32 width, u32 height, u32 stride, int pattern);
void DemoISR(void *callBackRef, void *pVideo);

// Image Buffer
u8 frameBuf[DISPLAY_NUM_FRAMES][DEMO_MAX_FRAME /* +1 */];
u8 *pFrames[DISPLAY_NUM_FRAMES]; //array of pointers to the frame buffers

DisplayCtrl dispCtrl;
XAxiVdma vdma;
XAxiVdma overlaydma;
INTC intc;

int main() {

	init_platform();

	xil_printf("Setup Video Mixer\n");
	// Enable Mixer
	int * mixer_width = (int *) 0x44A50010;  *mixer_width = 640;
	int * mixer_height = (int *) 0x44A50018;  *mixer_height = 480;
	int * mixer_layer_enable =  (int *) 0x44A50040;  *mixer_layer_enable = 0x3;

	// Mixer Layer config
	int * mixer_layer_alpha = (int *) 0x44A50200; *mixer_layer_alpha = 150;
	int * mixer_layer_startX = (int *) 0x44A50208; *mixer_layer_startX = 0;
	int * mixer_layer_startY = (int *) 0x44A50210; *mixer_layer_startY = 0;
	int * mixer_layer_width= (int *) 0x44A50218; *mixer_layer_width = 640;
	int * mixer_layer_stride = (int *) 0x44A50220; *mixer_layer_stride = 640*3;
	int * mixer_layer_height = (int *) 0x44A50228; *mixer_layer_height = 480;
	int * mixer_layer_scale = (int *) 0x44A50230; *mixer_layer_scale = 0;

	// Mixer start and auto start
	int * mixer_start = (int *) 0x44A50000; *mixer_start = 0x00000081;

	// HDMI Output
	xil_printf("Setup HDMI\n");
	DemoInitialize();

	//Now enable interrupts
	platform_enable_interrupts();

	xil_printf("Setup Test Pattern in Memory\n");
	DemoPrintTest(frameBuf[0], 640, 480, DEMO_STRIDE, 1);
	DemoPrintTest(frameBuf[1], 640, 480, DEMO_STRIDE, 0);

	while(1) {

	}

	cleanup_platform();
	return 0;
}

void DemoInitialize() {
	int Status;
	XAxiVdma_Config *overlayVDMAConfig;
	XAxiVdma_Config *vdmaConfig;
	int i;

	/*
	 * Initialize an array of pointers to the 3 frame buffers
	 */
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++) {
		pFrames[i] = frameBuf[i];
	}

	vdmaConfig = XAxiVdma_LookupConfig(VGA_VDMA_ID);
	if (!vdmaConfig) {
		xil_printf("No video DMA found for ID %d\r\n", VGA_VDMA_ID);
		return;
	}

	overlayVDMAConfig = XAxiVdma_LookupConfig(XPAR_AXIVDMA_1_DEVICE_ID);
	if (!overlayVDMAConfig) {
		xil_printf("No video DMA found for ID %d\r\n", XPAR_AXIVDMA_1_DEVICE_ID);
		return;
	}

	Status = XAxiVdma_CfgInitialize(&vdma, vdmaConfig, vdmaConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("VDMA Configuration Initialization failed %d\r\n", Status);
		return;
	}

	Status = XAxiVdma_CfgInitialize(&overlaydma, overlayVDMAConfig, overlayVDMAConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		xil_printf("Overlay VDMA Configuration Initialization failed %d\r\n", Status);
		return;
	}

	Status = DisplayInitialize(&dispCtrl, &vdma, &overlaydma,DISP_VTC_ID, DYNCLK_BASEADDR, pFrames, DEMO_STRIDE);
	if (Status != XST_SUCCESS) {
		xil_printf("Display Ctrl initialization failed during demo initialization%d\r\n", Status);
		return;
	}

	Status = DisplayStart(&dispCtrl);
	if (Status != XST_SUCCESS) {
		xil_printf("Couldn't start display during demo initialization%d\r\n", Status);
		return;
	}

	return;
}

void DemoISR(void *callBackRef, void *pVideo) {
	char *data = (char *) callBackRef;
	*data = 1; //set fRefresh to 1
}


/* ------------------------------------------------------------ */
/*						   Utilities							*/
/* ------------------------------------------------------------ */
void DemoPrintTest(u8 *frame, u32 width, u32 height, u32 stride, int pattern) {
	u32 xcoi, ycoi;
	u32 iPixelAddr;
	u8 wRed, wBlue, wGreen;
	u32 wCurrentInt;
	double fRed, fBlue, fGreen, fColor;
	u32 xLeft, xMid, xRight, xInt;
	u32 yMid, yInt;
	double xInc, yInc;

	switch (pattern) {

	case 0:

		xInt = width / 4; //Four intervals, each with width/4 pixels
		xLeft = xInt * 3;
		xMid = xInt * 2 * 3;
		xRight = xInt * 3 * 3;
		xInc = 256.0 / ((double) xInt); //256 color intensities are cycled through per interval (overflow must be caught when color=256.0)

		yInt = height / 2; //Two intervals, each with width/2 lines
		yMid = yInt;
		yInc = 256.0 / ((double) yInt); //256 color intensities are cycled through per interval (overflow must be caught when color=256.0)

		fBlue = 0.0;
		fRed = 256.0;
		for(xcoi = 0; xcoi < (width*3); xcoi+=3) {
			/*
			 * Convert color intensities to integers < 256, and trim values >=256
			 */
			wRed = (fRed >= 256.0) ? 255 : ((u8) fRed);
			wBlue = (fBlue >= 256.0) ? 255 : ((u8) fBlue);
			iPixelAddr = xcoi;
			fGreen = 0.0;
			for(ycoi = 0; ycoi < height; ycoi++) {

				wGreen = (fGreen >= 256.0) ? 255 : ((u8) fGreen);
				frame[iPixelAddr] = wRed;
				frame[iPixelAddr + 1] = wBlue;
				frame[iPixelAddr + 2] = wGreen;
				if (ycoi < yMid) {
					fGreen += yInc;
				} else {
					fGreen -= yInc;
				}

				/*
				 * This pattern is printed one vertical line at a time, so the address must be incremented
				 * by the stride instead of just 1.
				 */
				iPixelAddr += stride;
			}

			if (xcoi < xLeft) {
				fBlue = 0.0;
				fRed -= xInc;
			} else if (xcoi < xMid) {
				fBlue += xInc;
				fRed += xInc;
			} else if (xcoi < xRight) {
				fBlue -= xInc;
				fRed -= xInc;
			} else {
				fBlue += xInc;
				fRed = 0;
			}
		}
		/*
		 * Flush the framebuffer memory range to ensure changes are written to the
		 * actual memory, and therefore accessible by the VDMA.
		 */
		Xil_DCacheFlushRange((unsigned int) frame, DEMO_MAX_FRAME);
		break;
	case 1:

		xInt = width / 7; //Seven intervals, each with width/7 pixels
		xInc = 256.0 / ((double) xInt); //256 color intensities per interval. Notice that overflow is handled for this pattern.

		fColor = 0.0;
		wCurrentInt = 1;
		for(xcoi = 0; xcoi < (width*3); xcoi+=3) {

			/*
			 * Just draw white in the last partial interval (when width is not divisible by 7)
			 */
			if (wCurrentInt > 7) {
				wRed = 255;
				wBlue = 255;
				wGreen = 255;
			} else {
				if (wCurrentInt & 0b001)
					wRed = (u8) fColor;
				else
					wRed = 0;

				if (wCurrentInt & 0b010)
					wBlue = (u8) fColor;
				else
					wBlue = 0;

				if (wCurrentInt & 0b100)
					wGreen = (u8) fColor;
				else
					wGreen = 0;
			}

			iPixelAddr = xcoi;

			for(ycoi = 0; ycoi < height; ycoi++) {
				frame[iPixelAddr] = wRed;
				frame[iPixelAddr + 1] = wBlue;
				frame[iPixelAddr + 2] = wGreen;
				/*
				 * This pattern is printed one vertical line at a time, so the address must be incremented
				 * by the stride instead of just 1.
				 */
				iPixelAddr += stride;
			}

			fColor += xInc;
			if (fColor >= 256.0) {
				fColor = 0.0;
				wCurrentInt++;
			}
		}
		/*
		 * Flush the framebuffer memory range to ensure changes are written to the
		 * actual memory, and therefore accessible by the VDMA.
		 */
		Xil_DCacheFlushRange((unsigned int) frame, DEMO_MAX_FRAME);
		break;
	default :
		xil_printf("Error: invalid pattern passed to DemoPrintTest");
	}
}

