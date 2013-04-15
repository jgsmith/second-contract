class SC.Views.AreaList extends Backbone.View
  initialize: (options) ->
    @model.bind 'reset', @render, this
    @model.bind 'add', @appendNewArea, this
    @template = _.template $('#area-list-template').html()
    @domain = options.domain

  render: ->
    @$el.html @template
      domain: @domain
    @$ul = @$el.find("ul").first()
    for area in @model.models
      @appendNewArea area
    @el

  appendNewArea: (area) ->
    @$ul.append(new SC.Views.AreaListItem({model:area,domain:@domain}).render())

$("#new-area-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  model = SC.App.currentViews["#main"]?.model
  if model?
    area_name = $("#new-area-modal").find("input[name='name']").val()
    model.createArea area_name
  $("#new-area-modal").modal('hide')
