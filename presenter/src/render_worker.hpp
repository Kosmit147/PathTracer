#pragma once

#include <tracer/renderer.hpp>

#include <future>
#include <memory>
#include <stop_token>

#include "common.hpp"

namespace presenter {

enum class RenderStatus : u8
{
    InProgress,
    JustCompleted,
    Completed,
};

class RenderWorker
{
public:
    explicit RenderWorker(usize image_width, usize image_height, tracer::ObjectView world);
    ~RenderWorker();

    RenderWorker(const RenderWorker&) = delete;
    auto operator=(const RenderWorker&) = delete;
    RenderWorker(RenderWorker&&) = delete;
    auto operator=(RenderWorker&&) = delete;

    auto stop() -> void;
    auto restart() -> void;

    [[nodiscard]] auto poll_status() -> RenderStatus;
    [[nodiscard]] auto time_ms() const -> double;
    [[nodiscard]] auto progress() const -> i32;

    [[nodiscard]] auto image() const -> const auto& { return _image; }

private:
    tracer::Image _image;
    tracer::ObjectView _world;
    std::future<double> _result;
    std::stop_source _stop_source;
    double _time_ms{ 0.0 };

    // Put this value on a different cache line, because it's going to be written to by the render thread.
    std::unique_ptr<volatile i32> _progress{ std::make_unique<volatile i32>(0) };
};

} // namespace presenter
