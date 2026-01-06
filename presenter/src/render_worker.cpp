#include "render_worker.hpp"

#include <tracer/renderer.hpp>

#include <chrono>
#include <future>
#include <memory>
#include <stop_token>
#include <utility>

#include "common.hpp"
#include "timer.hpp"

namespace presenter {

namespace {

auto timed_render(const tracer::ImageView<glm::vec4>& image, tracer::ObjectSpan world, const tracer::Camera& camera,
                  const tracer::RenderParams& render_params, std::stop_token stop_token, volatile i32* progress)
    -> double
{
    auto timer = HighResolutionTimer{};
    timer.start();
    tracer::render(image, world, camera, render_params, std::move(stop_token), progress);
    return timer.elapsed_ms();
}

} // namespace

RenderWorker::RenderWorker(usize image_width, usize image_height, tracer::ObjectSpan world,
                           const tracer::Camera& camera, const tracer::RenderParams& render_params)
{
    restart(image_width, image_height, world, camera, render_params);
}

RenderWorker::~RenderWorker()
{
    stop();
}

auto RenderWorker::stop() -> void
{
    _stop_source.request_stop();

    if (_result.valid())
        _result.get();

    _stop_source = std::stop_source{};
}

auto RenderWorker::restart(usize image_width, usize image_height, tracer::ObjectSpan world,
                           const tracer::Camera& camera, const tracer::RenderParams& render_params) -> void
{
    stop();

    _image.resize(image_width, image_height);
    _result = std::async(std::launch::async, timed_render, _image.view(), world, camera, render_params,
                         _stop_source.get_token(), _progress.get());

    _time_ms = 0.0;
}

auto RenderWorker::poll_status() -> RenderStatus
{
    using namespace std::chrono_literals;

    if (!_result.valid())
        return RenderStatus::Completed;

    if (_result.wait_for(0ms) == std::future_status::ready)
    {
        _time_ms = _result.get();
        return RenderStatus::JustCompleted;
    }

    return RenderStatus::InProgress;
}

auto RenderWorker::time_ms() const -> double
{
    return _time_ms;
}

auto RenderWorker::progress() const -> i32
{
    return *_progress;
}

} // namespace presenter
