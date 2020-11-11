#include <switch.h>
#include <deko3d.h>

#include "nx_graphics.h"

#define FB_NUM 2
#define CMDMEMSIZE 0x1000

static u32 s_fbWidth, s_fbHeight;

static DkDevice s_device;
static DkMemBlock s_fbMemBlock, s_workMemBlock, s_cmdMemBlock;
static DkSwapchain s_swapchain;
static DkCmdBuf s_cmdBuf;
static DkCmdList s_cmdLists[FB_NUM];
static DkFence s_fence;
static DkQueue s_queue;

void graphicsInit(u32 width, u32 height)
{
    DkImageLayoutMaker imgLayoutMaker;
    DkMemBlockMaker memBlockMaker;

    // Create the device, which is the root object
    DkDeviceMaker deviceMaker;
    dkDeviceMakerDefaults(&deviceMaker);
    s_device = dkDeviceCreate(&deviceMaker);

    // Calculate layout for the framebuffers
    DkImageLayout fbLayout;
    dkImageLayoutMakerDefaults(&imgLayoutMaker, s_device);
    imgLayoutMaker.flags = DkImageFlags_UsagePresent;
    imgLayoutMaker.format = DkImageFormat_RGBA8_Unorm;
    imgLayoutMaker.dimensions[0] = s_fbWidth = width;
    imgLayoutMaker.dimensions[1] = s_fbHeight = height;
    dkImageLayoutInitialize(&fbLayout, &imgLayoutMaker);

    // Retrieve necessary size and alignment for the framebuffers
    uint32_t fbSize  = dkImageLayoutGetSize(&fbLayout);
    uint32_t fbAlign = dkImageLayoutGetAlignment(&fbLayout);
    fbSize = (fbSize + fbAlign - 1) &~ (fbAlign - 1);

    // Create a memory block that will host the framebuffers
    dkMemBlockMakerDefaults(&memBlockMaker, s_device, FB_NUM*fbSize);
    memBlockMaker.flags = DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image;
    s_fbMemBlock = dkMemBlockCreate(&memBlockMaker);

    // Initialize the framebuffers with the layout and backing memory we've just created
    DkImage fbImages[FB_NUM];
    DkImage const* swapchainImages[FB_NUM];
    for (unsigned i = 0; i < FB_NUM; i ++)
    {
        swapchainImages[i] = &fbImages[i];
        dkImageInitialize(&fbImages[i], &fbLayout, s_fbMemBlock, i*fbSize);
    }

    // Create a swapchain out of the framebuffers we've just initialized
    DkSwapchainMaker swapchainMaker;
    dkSwapchainMakerDefaults(&swapchainMaker, s_device, nwindowGetDefault(), swapchainImages, FB_NUM);
    s_swapchain = dkSwapchainCreate(&swapchainMaker);

    // Create a memory block for the linear framebuffer
    dkMemBlockMakerDefaults(&memBlockMaker, s_device, width*height*4);
    memBlockMaker.flags = DkMemBlockFlags_CpuCached | DkMemBlockFlags_GpuUncached;
    s_workMemBlock = dkMemBlockCreate(&memBlockMaker);

    // Create a memory block for the command lists
    dkMemBlockMakerDefaults(&memBlockMaker, s_device, CMDMEMSIZE);
    memBlockMaker.flags = DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached;
    s_cmdMemBlock = dkMemBlockCreate(&memBlockMaker);

    // Create a command buffer
    DkCmdBufMaker cmdBufMaker;
    dkCmdBufMakerDefaults(&cmdBufMaker, s_device);
    s_cmdBuf = dkCmdBufCreate(&cmdBufMaker);
    dkCmdBufAddMemory(s_cmdBuf, s_cmdMemBlock, 0, CMDMEMSIZE);

    // Define source for linear framebuffer copies
    const DkCopyBuf linearSrc = {
        .addr        = dkMemBlockGetGpuAddr(s_workMemBlock),
        .rowLength   = 0,
        .imageHeight = 0,
    };

    // Define rectangle for the copies
    const DkImageRect copyRect = {
        .x     = 0,     .y      = 0,      .z     = 0,
        .width = width, .height = height, .depth = 1,
    };

    // Record command lists for the copies
    for (unsigned i = 0; i < FB_NUM; i ++) {
        DkImageView tiledDst;
        dkImageViewDefaults(&tiledDst, &fbImages[i]);
        dkCmdBufCopyBufferToImage(s_cmdBuf, &linearSrc, &tiledDst, &copyRect, 0);
        dkCmdBufSignalFence(s_cmdBuf, &s_fence, false);
        s_cmdLists[i] = dkCmdBufFinishList(s_cmdBuf);
    }

    // Create a queue, to which we will submit our command lists
    DkQueueMaker queueMaker;
    dkQueueMakerDefaults(&queueMaker, s_device);
    queueMaker.flags = 0; // we will only use this queue for transferring
    s_queue = dkQueueCreate(&queueMaker);
}

void graphicsExit(void)
{
    // Make sure the queue is idle before destroying anything
    dkQueueWaitIdle(s_queue);

    // Destroy all the resources we've created
    dkQueueDestroy(s_queue);
    dkCmdBufDestroy(s_cmdBuf);
    dkMemBlockDestroy(s_cmdMemBlock);
    dkMemBlockDestroy(s_workMemBlock);
    dkSwapchainDestroy(s_swapchain);
    dkMemBlockDestroy(s_fbMemBlock);
    dkDeviceDestroy(s_device);
}

void* graphicsFrameBegin(u32* out_stride)
{
    // Ensure the GPU is not reading from the framebuffer
    dkFenceWait(&s_fence, -1);

    // Return information
    if (out_stride) *out_stride = s_fbWidth*4;
    return dkMemBlockGetCpuAddr(s_workMemBlock);
}

void graphicsFrameEnd(void)
{
    // Flush the linear framebuffer
    dkMemBlockFlushCpuCache(s_workMemBlock, 0, s_fbWidth*s_fbHeight*4);

    // Present a frame
    int slot = dkQueueAcquireImage(s_queue, s_swapchain);
    dkQueueSubmitCommands(s_queue, s_cmdLists[slot]);
    dkQueuePresentImage(s_queue, s_swapchain, slot);
}
