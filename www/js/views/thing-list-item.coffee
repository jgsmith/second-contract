class SC.Views.ThingListItem extends Backbone.View
  tagName: 'li'

  initialize: (options) ->
    #@template = _.template $('#thing-list-item-template').html()
    @model.bind 'change', @render, this
    @domain = options.domain
    @area = options.area
    @ward = options.ward

  render: ->
    switch @ward
      when "scene" then tmpl = _.template $('#scene-list-item-template').html()
      when "path" then tmpl = _.template $('#path-list-item-template').html()
      when "terrain" then tmpl = _.template $('#terrain-list-item-template').html()
      else tmpl = _.template $('#thing-list-item-template').html()
    json = @model.toJSON()
    json.domain = @domain
    json.area = @area
    json.ward = @ward
    @$el.html tmpl json
    @el
