class SC.Views.VerbList extends Backbone.View
  initialize: ->
    @model.bind 'reset', @render, this
    @model.bind 'add', @appendNewVerb, this
    @template = _.template $('#verb-list-template').html()

  render: ->
    @$el.html @template {}
    @$ul = @$el.find("ul").first()
    for verb in @model.models
      @appendNewVerb verb
    @el

  appendNewVerb: (verb) ->
    @$ul.append(new SC.Views.VerbListItem({model:verb}).render())

$("#new-verb-modal").find(".btn-primary").click (e) ->
  e.preventDefault()
  verbs = $("#new-verb-modal").find("input[name='verbs']").val()
  if verbs?
    verbs = verbs.split(/\s*,\s*/)
    console.log "Verbs:", verbs
    data =
      verb: verbs
      disabled: true
    console.log "Creating verb"
    SC.Collections.verbs.create data,
      wait: true
    
  $("#new-verb-modal").modal('hide')
