class SC.Views.Area extends Backbone.View
  initialize: ->
    @template = _.template $('#area-template').html()
    @listenTo @model, 'change', @render

  events:
    'click .save': 'save'

  render: ->
    json = @model.toJSON()
    @$el.html @template json
    sceneList = new SC.Views.ThingList
      model: @model.scenes
    wardList = new SC.Views.WardList
      model: @model.wards
    @$el.find("#area-scenes").html sceneList.render()
    @$el.find("#area-templates").html wardList.render()
    later =>
      el = $("#area-map-raphael-canvas")
      @mapCanvas = Raphael("area-map-raphael-canvas", el.width(), el.width() * 2 / 3)
      $(window).resize =>
        @mapCanvas.setSize(el.width(), el.width()*2/3)
    @el

  save: (e) ->
    e.preventDefault()

$("#new-area-scene-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  scene_id = $("#new-area-scene-modal").find("input[name='id']").val()
  area = SC.App.currentViews["#main"]?.model
  if area?
    area.scenes.create { id: scene_id },
      success: =>
        window.location.replace("#worldlib/scene/#{@domain}/#{@id}/#{scene_id}")
      
  $("#new-area-scene-modal").modal('hide')

$("#new-area-ward-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  ward_name = $("#new-area-ward-modal").find("input[name='name']").val()
  area = SC.App.currentViews["#main"]?.model
  if area?
    area.wards.create { id: ward_name }
  $("#new-area-ward-modal").modal('hide')
